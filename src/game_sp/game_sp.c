//
// Created by jw910731 on 2021/6/13.
//

#include "game_sp.h"

#include "../game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
static enum sanjuan_card supply[SANJUAN_MAX_CARD];
static enum sanjuan_card discard[SANJUAN_MAX_CARD];

/*
 * Runtime linking function
 */
static void game_init();
static void game_free();
static void game_start();
static int32_t player_count_getter();

void sanjuan_gamesp_runtime_link(){
    sanjuan_game_init = game_init;
    sanjuan_game_free = game_free;
    sanjuan_game_start = game_start;
    sanjuan_game_draw;
    sanjuan_game_discard;
    sanjuan_game_player_count = player_count_getter;
}

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
}

static void game_free(){
    printf("GameSP End (TODO)\n");
}

static void game_start(){
    while(1){
        for(int32_t gov_cnt = 0 ; gov_cnt < player_count ; ++gov_cnt){ // governor take turn
            for(int32_t round_counter = 0 ; round_counter < ((player_count > 2)?player_count:3) ; ++round_counter){ // select
                // the role selecting player id
                int32_t selector = (gov_cnt + round_counter) % player_count;
                enum sanjuan_role role = client_metas[selector].choose_role();
                // take turn to perform role
                for(int cnt = 0, i = selector; cnt < player_count ; i = (i+1) % player_count){
                    // TODO: perform role
                }
            }
        }
    }
}

static int32_t player_count_getter(){
    return player_count;
}