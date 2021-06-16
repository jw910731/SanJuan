//
// Created by jw910731 on 2021/6/13.
//

#ifndef SANJUAN_ROOT_GAME_SP_H
#define SANJUAN_ROOT_GAME_SP_H

#include "../game.h"

/** @struct
 * Card that is in hand, table or chapel
 */
struct deck_card{
    enum sanjuan_card card;///< Represent card type
    int32_t storage; ///<  Card type independent storage space
};

/** @struct
 * Stores player state
 */
struct player{
    struct deck_card hand[SANJUAN_MAX_CARD];
    struct deck_card table[SANJUAN_MAX_CARD];
    struct deck_card chapel[SANJUAN_MAX_CARD];
};

/**
 * Perform runtime linking to link gameSP to game component
 */
void sanjuan_gamesp_runtime_link();

/**
 * Parse arg from CLI
 * Used to fill in information to gameSP
 * @param saveptr the <code>saveptr</code> of <code>strtok_r</code> for continuous call of <code>strtok_r</code>
 */
int32_t sanjuan_gamesp_arg_parse(char **);

#endif //SANJUAN_ROOT_GAME_SP_H
