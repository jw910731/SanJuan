//
// Created by jw910731 on 2021/6/13.
//

#ifndef SANJUAN_ROOT_GAME_SP_H
#define SANJUAN_ROOT_GAME_SP_H

#include "../game.h"

/**
 * Stores player state
 */
struct player{
    enum sanjuan_card hand[SANJUAN_MAX_CARD]; // NULL Terminate
    struct sanjuan_table_card table[12];
    enum sanjuan_card chapel[SANJUAN_MAX_CARD]; // NULL Terminate
};

/**
 * Perform runtime linking to link gameSP to game component
 */
void sanjuan_gamesp_runtime_link();

/**
 * Parse arg from CLI
 * Used to fill in information to gameSP
 * @param saveptr the <code>saveptr</code> of <code>strtok_r</code> for continuous call of <code>strtok_r</code>
 * @return 1 for fail, 0 for successfully setup game
 */
int32_t sanjuan_gamesp_arg_parse(char **);

#endif //SANJUAN_ROOT_GAME_SP_H
