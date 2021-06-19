//
// Created by jw910731 on 2021/6/13.
//

/** @file
 *  This file contain the Single player game's game component implementation.
 *  It take care of all client that will join the game
 */

#include "game_sp.h"

#include "../util/util.h"
#include "../game.h"
#include "../shell.h"
#include "dummy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/select.h>

/************************
 *  Internal Game Data  *
 ***********************/

/*
 * Clients
 */
static struct sanjuan_client_meta client_metas[4];

/*
 * All joining player data
 */
static struct player player_data[4];
static int32_t player_count;

/*
 * Card supply and discard
 */
static int32_t supply_count;
static enum sanjuan_card supply[SANJUAN_MAX_CARD];
static int32_t discard_count;
static enum sanjuan_card discard[SANJUAN_MAX_CARD];

/*
 * Runtime linking function
 */
static void game_init();
static void game_free();

_Noreturn static void game_start();
static int32_t player_count_getter();
static enum sanjuan_card draw();
static void discard_card(enum sanjuan_card);
static enum sanjuan_card *get_hand(int32_t id);
static struct sanjuan_table_card *get_table(int32_t id);
static void emplace_hand(int32_t id, enum sanjuan_card card);
static void remove_hand(int32_t id, int32_t idx);
static void place_table(int32_t id, enum sanjuan_card card, int32_t idx);

void sanjuan_gamesp_runtime_link(){
    sanjuan_game_init = game_init;
    sanjuan_game_free = game_free;
    sanjuan_game_start = game_start;
    sanjuan_game_draw = draw;
    sanjuan_game_discard = discard_card;
    sanjuan_game_player_count = player_count_getter;
    sanjuan_game_get_hand = get_hand;
    sanjuan_game_get_table = get_table;
    sanjuan_game_player_emplace_hand = emplace_hand;
    sanjuan_game_player_remove_hand = remove_hand;
    sanjuan_game_player_place_table = place_table;
}

/*
 * Internal helper function prototype
 */
static void card_shuffle(enum sanjuan_card *cards, size_t n);
static int32_t card_list_len(enum sanjuan_card *cards);


int32_t sanjuan_gamesp_arg_parse(char **saveptr){
    char *it = strtok_r(NULL, " ", saveptr);
    if(it == NULL) {
        printf("Command usage error!\n");
        return 1;
    }

    int player_count_tmp = 0;
    if(strcmp(it, "help") == 0){
        static char *help = "game start help\n"
                            "Usage: game start [AI Type...]\n"
                            "Use at least 1 AI at most 3 AIs to play with you!\n"
                            "Available AI Type:\n"
                            "    Random: AI that do random available action.\n";
        printf("%s", help);
        return 1;
    }
    else{
        // setup shell client (human player)
        struct sanjuan_client_meta *client = &client_metas[player_count_tmp];
        memcpy(client, &sanjuan_shell_client_meta, sizeof(struct sanjuan_client_meta));
        client->id = player_count_tmp;
        client->init(client); // init client meta
        ++player_count_tmp;

        static const struct sanjuan_client_meta *ai_metas[] = {&sanjuan_dummyAI_client_meta};
        static const char *ai_name[] = {"dummy"};
        static const int32_t ai_count = (int32_t)(sizeof(ai_name)/sizeof(*ai_name));

        for( ; it != NULL && player_count_tmp <= 4; it = strtok_r(NULL, " ", saveptr)){
            if(player_count_tmp >= 4) {
                bzero(client_metas, sizeof(client_metas));
                printf("Too much AI player!\n");
                return 1;
            }
            int32_t i;
            for(i = 0 ; i < ai_count; ++i){
                if(strcmp(ai_name[i], it) == 0){ // if matched ai
                    client = &client_metas[player_count_tmp];
                    memcpy(client, ai_metas[i], sizeof(struct sanjuan_client_meta));
                    client->id = player_count_tmp;
                    client->init(client); // init client meta
                    ++player_count_tmp;
                    break;
                }
            }
            if(i >= ai_count){
                bzero(client_metas, sizeof(client_metas));
                printf("Failed to match ai %s. Please use help to checkout available AI.\n", it);
                return 1;
            }
        }
        if(player_count_tmp <= 1){
            bzero(client_metas, sizeof(client_metas));
            printf("No enough AI to play with you, you need at least 1 AI to play with you in single player mode.\n");
            return 1;
        }
    }
    // setup players
    player_count = player_count_tmp;
    return 0;
}

static void game_init(){
    // Initialize card supply
    int32_t cnt = 0;
    for(int32_t i = 0 ; i < 29 ; ++i){
        for(int32_t j = 0 ; j < init_card_count[i] ; ++i){
            supply[cnt++] = i;
        }
    }
    supply_count = cnt;
    discard_count = 0;
    card_shuffle(supply, supply_count);

    // Initialize player data
    for(int i = 0 ; i < player_count ; ++i){
        struct player *p = &player_data[i];
        p->hand[0] = CARD_INVALID;
        for(int j = 0 ; j < 12 ; ++j){
            p->table[j] = (struct sanjuan_table_card){CARD_INVALID, false};
        }
        p->chapel[0] = CARD_INVALID;
    }
}

static void game_free(){
    printf("GameSP End (TODO)\n");
}

_Noreturn static void game_start(){
    while(1){
        // TODO: discard excess card (> 7 or > 12)

        for(int32_t gov_cnt = 0 ; gov_cnt < player_count ; ++gov_cnt){ // governor take turn
            int8_t ava_role = 0x1f;
            for(int32_t round_counter = 0 ; round_counter < ((player_count > 2)?player_count:3) ; ++round_counter){ // inside round
                // the role selecting player id
                int32_t selector = (gov_cnt + round_counter) % player_count;
                struct sanjuan_client_meta *role_picker = &client_metas[selector];
                role_picker->pre_choose_role(role_picker, ava_role);

                enum sanjuan_role role;
                if(role_picker->fd > -1){ // block input stream
                    enum sanjuan_role client_ret = ROLE_INVALID;
                    while(client_ret == ROLE_INVALID){
                        fd_set fds;
                        FD_ZERO(&fds);
                        FD_SET(role_picker->fd, &fds);
                        int ret = select(1, &fds, NULL, NULL, NULL);

                        // there should be no error
                        assert(ret > 0);
                        // input stream available
                        if(ret > 0){
                            client_ret = role_picker->choose_role(role_picker);
                        }
                    }
                    role = client_ret;
                }
                else{
                    role = role_picker->choose_role(role_picker);
                }
                // mark selected role
                ava_role = (int8_t)(ava_role & ((int8_t)(~(1<<role))));
                // take turn to perform role
                for(int cnt = 0, i = selector; cnt < player_count ; i = (i+1) % player_count, ++cnt){
                    struct sanjuan_client_meta *performer = &client_metas[i];
                    performer->pre_perform_role(performer, role, i);

                    // perform role
                    if(role_picker->fd > -1){ // block input stream
                        int32_t client_ret = 0;
                        while(client_ret != 1){
                            fd_set fds;
                            FD_ZERO(&fds);
                            FD_SET(role_picker->fd, &fds);
                            int ret = select(1, &fds, NULL, NULL, NULL);

                            // there should be no error
                            assert(ret > 0);
                            // input stream available
                            if(ret > 0){
                                client_ret = performer->perform_role(role_picker);
                            }
                        }
                    }
                    else{
                        performer->perform_role(role_picker);
                    }
                }

                // TODO: if builder -> check if game ends
            } // end round internal loop
            // TODO: perform chapel
        } // end switch governor loop
    }
}

static int32_t player_count_getter(){
    return player_count;
}

static enum sanjuan_card draw(){
    // refill supply
    if(supply_count <= 0){
        // there is no way to get all card on player hands
        assert(discard_count > 0);
        memcpy(supply, discard, sizeof(enum sanjuan_card) * discard_count);
        supply_count = discard_count;
        discard_count = 0;
        card_shuffle(supply, supply_count);
    }
    return supply[--supply_count];
}

static void discard_card(enum sanjuan_card card){
    discard[discard_count++] = card;
    // prevent discard larger than MAX CARD (may be bug in client)
    assert(discard_count <= SANJUAN_MAX_CARD);
}

static void card_shuffle(enum sanjuan_card *cards, size_t n){
    sanjuan_shuffle(cards, n, sizeof(enum sanjuan_card));
}

static enum sanjuan_card *get_hand(int32_t id){
    if(id < 0 || id >= player_count) return NULL;
    return player_data[id].hand;
}

static struct sanjuan_table_card *get_table(int32_t id){
    if(id < 0 || id >= player_count) return NULL;
    return player_data[id].table;
}

static void emplace_hand(int32_t id, enum sanjuan_card card){
    for(int32_t i = 0 ; i < SANJUAN_MAX_CARD ; ++i) {
        if (player_data[id].hand[i] == CARD_INVALID) {
            player_data[id].hand[i] = card;
            player_data[id].hand[i + 1] = CARD_INVALID;
            break;
        }
    }
}

static void remove_hand(int32_t id, int32_t idx){
    int32_t len = card_list_len(player_data[id].hand);
    if(idx < 0 || idx >= len) return;
    memcpy(&player_data[id].hand[idx], &player_data[id].hand[idx+1], sizeof(enum sanjuan_card) * (len - idx - 1));
    player_data[id].hand[len-1] = CARD_INVALID;
}

static void place_table(int32_t id, enum sanjuan_card card, int32_t idx){
    if(idx < 0 || idx >= 12) return;
    player_data[id].table[idx] = (struct sanjuan_table_card){card, false};
}

static int32_t card_list_len(enum sanjuan_card *cards){
    for(int32_t i = 0 ; i < SANJUAN_MAX_CARD ; ++i) {
        if (cards[i] == CARD_INVALID) {
            return i;
        }
    }
}