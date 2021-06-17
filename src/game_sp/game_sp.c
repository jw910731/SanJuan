//
// Created by jw910731 on 2021/6/13.
//

#include "game_sp.h"

#include "../util/util.h"
#include "../game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
static struct player p[4];
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
static void game_start();
static int32_t player_count_getter();
static enum sanjuan_card draw();
static void discard_card(enum sanjuan_card);

void sanjuan_gamesp_runtime_link(){
    sanjuan_game_init = game_init;
    sanjuan_game_free = game_free;
    sanjuan_game_start = game_start;
    sanjuan_game_draw = draw;
    sanjuan_game_discard = discard_card;
    sanjuan_game_player_count = player_count_getter;
}

/*
 * Internal helper function prototype
 */
static void card_shuffle(enum sanjuan_card *cards, size_t n);


int32_t sanjuan_gamesp_arg_parse(char **saveptr){
    char *it = strtok_r(NULL, " ", saveptr);
    if(it == NULL) return 1;
    int player_count_tmp = 1;
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

        ++player_count_tmp;
        for(it = strtok_r(NULL, " ", saveptr) ; it != NULL && player_count_tmp <= 4; it = strtok_r(NULL, " ", saveptr)){
            if(player_count_tmp >= 4) {
                printf("Too much AI player!\n");
                return 1;
            }
            ++player_count_tmp;
        }
    }
    // setup players
    player_count = player_count_tmp;

    // TODO: setup client meta and assign id to it
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
}

static void game_free(){
    printf("GameSP End (TODO)\n");
}

static void game_start(){
    while(1){
        for(int32_t gov_cnt = 0 ; gov_cnt < player_count ; ++gov_cnt){ // governor take turn
            int8_t ava_role = 0x1f;
            for(int32_t round_counter = 0 ; round_counter < ((player_count > 2)?player_count:3) ; ++round_counter){ // inside round
                // the role selecting player id
                int32_t selector = (gov_cnt + round_counter) % player_count;
                enum sanjuan_role role = client_metas[selector].choose_role(ava_role);
                // mark selected role
                ava_role = (int8_t)(ava_role & ((int8_t)(~(1<<role))));
                // take turn to perform role
                for(int cnt = 0, i = selector; cnt < player_count ; i = (i+1) % player_count, ++cnt){
                    // TODO: perform role
                }
            }
        }
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