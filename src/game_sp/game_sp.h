//
// Created by jw910731 on 2021/6/13.
//

#ifndef SANJUAN_ROOT_GAME_SP_H
#define SANJUAN_ROOT_GAME_SP_H

#include "../game.h"

/*
 * Card not in card supply
 */
struct deck_card{
    /*
     * Represent card type
     */
    enum sanjuan_card card;

    /*
     * Card type independent storage space
     */
    int32_t storage;
};

/*
 * Player util struct
 */
struct player{
    struct deck_card hand[SANJUAN_MAX_CARD];
    struct deck_card table[SANJUAN_MAX_CARD];
    struct deck_card chapel[SANJUAN_MAX_CARD];
};

/*
 * Perform runtime linking to link gameSP to game component
 */
void sanjuan_gamesp_runtime_link();

/*
 * Parse arg from CLI
 * Used to fill in information to gameSP
 */
int32_t sanjuan_gamesp_arg_parse(char **);

#endif //SANJUAN_ROOT_GAME_SP_H
