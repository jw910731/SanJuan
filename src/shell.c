//
// Created by jw910731 on 2021/6/12.
//

#include "shell.h"
#include "game.h"
#include "game_sp/game_sp.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

static int8_t ava_role; // (globale state) Currently available state
static enum sanjuan_role role_cb_state;
static int32_t state; // Callback set this to zero to refer not ready, non zero to ready with information inside
static struct sanjuan_client_meta *thisMeta;
static int32_t id_cb; // for callback to acknowledge role chooser id

/*
 * Helper function declaration
 */
static int32_t card_list_len(enum sanjuan_card *cards);
static bool hand_contain(enum sanjuan_card * hand, enum sanjuan_card card);
static bool table_contain(struct sanjuan_table_card * table, enum sanjuan_card card);
static int32_t general_cmd_solver(char *); ///< solve general command, return 1 for success 0 for fail
static const char *general_cmd_help = "    hand: checkout your current hand cards.\n"
                                      "    table <player id>: checkout other player's table card\n";
static void print_used_role_helper(int8_t);
static bool empty_string(const char *s);
static void crole_line_cb(char *);
static void prole_line_cb(char *);
static void chapel_line_cb(char *);

/*
 * Client Meta exposing function prototype
 */
static int32_t init(struct sanjuan_client_meta *meta);
static void free_client(struct sanjuan_client_meta *meta);
static void pre_choose_role(struct sanjuan_client_meta *meta, const int8_t bitmask);
static enum sanjuan_role choose_role(struct sanjuan_client_meta *meta);
static int32_t idle(struct sanjuan_client_meta *meta);
static void pre_perform_role(struct sanjuan_client_meta *meta, enum sanjuan_role role, int32_t id);
static int32_t perform_role(struct sanjuan_client_meta *meta);
static int32_t perform_chapel(struct sanjuan_client_meta *meta);

const struct sanjuan_client_meta sanjuan_shell_client_meta={
        .fd = 0,
        .init = init,
        .free = free_client,
        .pre_choose_role = pre_choose_role,
        .choose_role = choose_role,
        .idle = idle,
        .pre_perform_role = pre_perform_role,
        .perform_role = perform_role,
        .perform_chapel = perform_chapel,
};

struct client_state{
};

static int32_t general_cmd_solver(char *s){
    static const char *cmds[]={"hand", "table"};
    char *saveptr;
    char *it = strtok_r(s, " ", &saveptr);
    if(it == NULL) return 0;
    int32_t cmd;
    for(cmd = 0 ; cmd < ((int32_t)(sizeof(cmds)/sizeof(*cmds))) ; ++cmd){
        if(strcmp(cmds[cmd], it) == 0){
            break;
        }
    }
    switch(cmd){
        case 0: {
            it = strtok_r(NULL, " ", &saveptr);
            if (it != NULL) return 0;
            else {
                enum sanjuan_card *cards = sanjuan_game_get_hand(thisMeta->id);
                printf("Current hand cards: ");
                bool flag = true;
                for (int32_t i = 0; i < SANJUAN_MAX_CARD; ++i) {
                    if (cards[i] == CARD_INVALID) break;
                    flag = false;
                    printf("%s, ", card_name[cards[i]]);
                }
                if (flag) printf("You had no card!");
                putchar('\n');
            }
            break;
        }
        case 1: {
            it = strtok_r(NULL, " ", &saveptr);
            if (it == NULL) return 0;
            char *endptr;
            int32_t id = (int32_t) strtol(it, &endptr, 10);
            if(*endptr) return 0; // if not interpret number to null terminator => input error
            struct sanjuan_table_card *tables = sanjuan_game_get_table(id);
            printf("The Player%d current table: ");
            bool flag = true;
            for (int32_t i = 0; i < SANJUAN_MAX_CARD; ++i) {
                if (tables[i].class == CARD_INVALID) break;
                flag = false;
                printf("%s", card_name[tables[i].class]);
                if(card_isPurple[tables[i].class]){
                    printf("(%d)", tables[i].goods);
                }
                printf(", ");
            }
            if (flag) printf("there is no card!");
            putchar('\n');
            break;
        }
        default:
            return 0;
    }
    return 1;
}

static void crole_line_cb(char * cmd){
    size_t len = strlen(cmd);
    char *copy = calloc(len+1, sizeof(char));
    if(empty_string(cmd)) goto cleanup;
    strncpy(copy, cmd, len);
    int32_t ret = general_cmd_solver(copy);

    if(ret){
        goto cleanup;
    }
    strncpy(copy, cmd, len);
    if(strcmp(copy, "help") == 0){
        printf("Choose role help:\n"
               "    available: choose available roles.\n"
               "    Builder, Councillor, Producer, Prospector, Trader: select these role (choose only when available!)\n" );
        printf("%s", general_cmd_help);
        goto cleanup;
    }
    if(strcmp(copy, "available") == 0){
        print_used_role_helper(ava_role);
        goto cleanup;
    }
    for(int i = 0 ; i < 5 ; ++i){
        if(strcasecmp(copy, rolename[i]) == 0){
            if(ava_role & (1<<i))
                role_cb_state = i;
            else
                printf("This role is unavailable.\n");
            goto cleanup;
        }
    }
    printf("Command or role not found!\n");
    cleanup:
    free(copy);
    free(cmd);
    // check is callback done
    if(role_cb_state != ROLE_INVALID){
        rl_callback_handler_remove();
    }
}

static int32_t card_list_len(enum sanjuan_card *cards){
    for(int32_t i = 0 ; i < SANJUAN_MAX_CARD ; ++i) {
        if (cards[i] == CARD_INVALID) {
            return i;
        }
    }
}

static void doProspector() {
    if (id_cb == thisMeta->id) { // you are privileged
        enum sanjuan_card drawn = sanjuan_game_draw();
        printf("[Prospector] You had activate prospector privilege: card %s drawn to hand.\n", card_name[drawn]);
        sanjuan_game_player_emplace_hand(thisMeta->id, drawn);
    }

    // TODO: implement Library here

    // active gold mine
    if (table_contain(sanjuan_game_get_table(thisMeta->id), CARD_GOLD_MINE)) {
        printf("[Prospector] Gold mine activating!\n");
        enum sanjuan_card drawn[4];
        for (int32_t i = 0; i < 4; ++i)
            drawn[i] = sanjuan_game_draw();
        printf("[Prospector] You had drawn card: ");
        for (int32_t i = 0; i < 4; ++i)
            printf("%s, ", card_name[drawn[i]]);

        // perform gold mine action
        int32_t cnt[6] = {0};
        for (int32_t i = 0; i < 4; ++i) {
            ++cnt[card_cost[drawn[i]] - 1];
        }
        bool flag = true;
        for (int32_t i = 0; i < 6; ++i) {
            if (cnt[i] > 1) {
                flag = false;
            }
        }
        if (flag) { // TODO: Convention breaking! Figure out a way to avoid this!
            printf("[Prospector] Gold mine activation success! Keep one of the drawn card.\n");
            int32_t index = -1;
            while (index >= 0) {
                char *selection = readline("select one card>");
                char *err;
                int32_t tmp = (int32_t) strtol(selection, &err, 10);
                if (*err) continue;
                if (tmp < 0 || tmp > 4) continue;
                index = tmp;
                free(selection);
            }
            printf("[Prospector] Keep card %s. Other cards returns to discard stack.\n", card_name[drawn[index]]);
            sanjuan_game_player_emplace_hand(thisMeta->id, drawn[index]);
            for (int32_t i = 0; i < 4; ++i) {
                if (i == index) continue;
                sanjuan_game_discard(drawn[i]);
            }
        }
    }
}

static void doBuilder(char *cmd){
    size_t len = strlen(cmd);
    char *copy = calloc(len+1, sizeof(char));
    if(empty_string(cmd)) return;
    strncpy(copy, cmd, len);
    int32_t ret = general_cmd_solver(copy);

    if(ret){
        return;
    }
    strncpy(copy, cmd, len);
    if(strcmp(copy, "help") == 0){
        printf("Builder help:\n"
               "    build <hand card index> <table index>: build the designated card at table index.\n");
        printf("%s", general_cmd_help);
        return;
    }
    else{
        // discount!
        int32_t discounts = 0;
        if(id_cb == thisMeta->id) ++discounts;
        // TODO: implement global discount card

        char *saveptr;
        char *it = strtok_r(copy, " ", &saveptr);
        if(strcmp(it, "build") != 0){
            printf("Command error. See help.\n");
            return;
        }
        // build!
        it = strtok_r(NULL, " ", &saveptr);
        if(it == NULL){ // no argument is given after build command
            printf("Command error. See help.\n");
            return;
        }
        // parse
        char *err;
        int32_t hidx = (int32_t)strtol(it, &err, 10);
        if(*err) {
            printf("Index error.\n");
        }
        enum sanjuan_card *hands = sanjuan_game_get_hand(thisMeta->id);
        if(hidx < 0 || hidx >= card_list_len(hands)){
            printf("Index out of bound.\n");
        }

        // building position
        it = strtok_r(NULL, " ", &saveptr);
        if(it == NULL){ // no argument is given after build command
            printf("Command error. See help.\n");
            return;
        }
        int32_t tidx = (int32_t)strtol(it, &err, 10);
        if(*err) {
            printf("Index error.\n");
        }
        if(hidx < 0 || hidx >= 12){
            printf("Index out of bound.\n");
        }

        if(strtok_r(NULL, " ", &saveptr) != NULL){ // no extra argument check
            printf("Command error. See help.\n");
            return;
        }

        // start building
        struct sanjuan_table_card *table = sanjuan_game_get_table(thisMeta->id);
        // TODO: calculate building cost (with discounts)
        // TODO: player pay building cost
        // disable override card when no crane
        if(!table_contain(table, CARD_CRANE)){
            if(table[tidx].class != CARD_INVALID){
                printf("You can't build on another building.\n");
                return;
            }
        }
        sanjuan_game_player_place_table(thisMeta->id, hands[hidx], tidx);
        sanjuan_game_discard(hands[hidx]);
        state = 1;
        return;
    }
}

static void doCouncillor(char *cmd){

}

static void doProducer(char *cmd){

}

static void doTrader(char *cmd){

}

static void prole_line_cb(char * cmd){
    switch (role_cb_state) {
        case ROLE_BUILDER:
            doBuilder(cmd);
            break;
        case ROLE_COUNCILLOR:
            doCouncillor(cmd);
            break;
        case ROLE_PRODUCER:
            doProducer(cmd);
            break;
        case ROLE_TRADER:
            doTrader(cmd);
            break;
        default:
            break;
    }

    free(cmd);

    if(state) {
        rl_callback_handler_remove();
        state = 0;
    }
}

static void chapel_line_cb(char * cmd){
    free(cmd);
}


static int32_t init(struct sanjuan_client_meta *meta){
    meta->context = calloc(sizeof(struct client_state), 1);
    meta->fd = 0;
    return 1;
}

static void free_client(struct sanjuan_client_meta *meta){
    free(meta->context);
    // TODO: may need to uninstall non-blocking GNU readline
}

static void pre_choose_role(struct sanjuan_client_meta *meta, const int8_t bitmask){
    thisMeta = meta;
    ava_role = bitmask;
    role_cb_state = ROLE_INVALID;
    print_used_role_helper(bitmask);
    rl_callback_handler_install ("Choose Role >", crole_line_cb);
}

static enum sanjuan_role choose_role(struct sanjuan_client_meta *meta){
    // call readline
    rl_callback_read_char();
    return role_cb_state;
}

static int32_t idle(struct sanjuan_client_meta *meta){
    printf("This is not implemented and is only a placeholder\n");
    return 1;
}

static void pre_perform_role(struct sanjuan_client_meta *meta, enum sanjuan_role role, int32_t id){
    thisMeta = meta;
    role_cb_state = role;
    state = 0;
    id_cb = id;

    // Prospector will be resolve here
    if(role == ROLE_PROSPECTOR){
        doProspector();
        state = 1;
    }
    else{
        static const char *prompts[]={"Builder>", "Councillor>", "Producer>", "Prospector>", "Trader>"};
        rl_callback_handler_install(prompts[role], prole_line_cb);
    }
}

static int32_t perform_role(struct sanjuan_client_meta *meta){
    // avoid prospector being process again
    if(!state){
        rl_callback_read_char();
    }
    return state;
}

static int32_t perform_chapel(struct sanjuan_client_meta *meta){
    thisMeta = meta;
    // TODO: do readline

    if(state) {
        // cleanup
        state = 0;
        return state;
    }
    return 0;
}

static void print_used_role_helper(int8_t ava_role){
    printf("Available Roles: ");
    bool flag = true;
    for(int i = 0 ; i < 5 ; ++i){
        if(ava_role & (1<<i)){
            flag = false;
            printf("%s ", rolename[i]);
        }
    }
    if(flag){
        printf("Error! No available role");
    }
    putchar('\n');
}

static bool hand_contain(enum sanjuan_card * hand, enum sanjuan_card card){
    for(int32_t i = 0 ; i < SANJUAN_MAX_CARD ; ++i){
        if(hand[i] == CARD_INVALID) return false;
        if(hand[i] == card) return true;
    }
    return false;
}
static bool table_contain(struct sanjuan_table_card * table, enum sanjuan_card card){
    for(int32_t i = 0 ; i < SANJUAN_MAX_CARD ; ++i){
        if(table[i].class == CARD_INVALID) return false;
        if(table[i].class == card) return true;
    }
    return false;
}

static bool empty_string(const char *s){
    bool flag = true;
    for(const char *it = s ; *it ; ++it){
        flag = false;
        if(!isspace(*it)){
            return false;
        }
    }
    return flag;
}

// Forbidden to input empty string, should be filtered out before calling
static int32_t boostrap_command_parser(char *s){
    static const char *help_msg = "SanJuan Shell help message\n"
                                  "    help / ? : Print this help message.\n"
                                  "    game <option> : To start the game or get game related information.\n"
                                  "    exit : Exit shell.\n"
                                  "To get detail help message of command. Use <command> -h\n";
    char *saveptr;
    char *it = strtok_r(s, " ", &saveptr);

    static const char *command_list[] = {"help", "?", "game", "exit"};
    int cmd = -1;
    for(int32_t i  = 0 ; i < (sizeof(command_list) / sizeof(*command_list)) ; ++i){
        if(strcmp(it, command_list[i]) == 0){
            cmd = i;
            break;
        }
    }

    switch(cmd){
        case 0:
        case 1:
            printf("%s", help_msg);
            break;
        case 2: {
            it = strtok_r(NULL, " ", &saveptr);
            if(strcmp(it, "help") == 0){
                static const char *game_help =  "game command help\n"
                                                "    game start: start a single player game.\n"
                                                "    game connect: connect to a remote server to start game. (not implemented)\n";
                printf("%s", game_help);
                break;
            }
            if(strcmp(it, "start") == 0){
                // parse arg and setup runtime link
                if(sanjuan_gamesp_arg_parse(&saveptr) == 0){
                    sanjuan_gamesp_runtime_link();
                    return 3; // game ready to boostrap
                }
            }
            if(strcmp(it, "connect") == 0){
                printf("Currently not implemented.\n");
            }
            return 1; // continue to listen cmd
        }
        case 3:
            return 2;
        default:
            return 0; // error
    }
    return 1; // continue to listen cmd
}

void sanjuan_shell_boostrap(){
    // setup GNU readline
    stifle_history(100);
    rl_bind_key ('\t', rl_insert);

    // mainloop
    char *line = NULL;
    bool loop_flag = true;
    while(loop_flag){
        line = readline("SanJuan sh>");
        if(!line) break;

        if(!empty_string(line)){
            // parse command
            int32_t stat = boostrap_command_parser(line);
            if(stat <= 0){ // error
                printf("Unknown command. Get help message by typing \"help\". \n");
            }
            if(stat == 2){ // exit
                loop_flag = false;
            }
            if(stat == 3){ // successfully boostrap game
                sanjuan_game_init();
                sanjuan_game_start();
                sanjuan_game_free();
            }

            // save history
            add_history(line);
        }

        // cleanup line
        free(line);
        line = NULL;
    }

    // clean GNU readline
    clear_history ();
}