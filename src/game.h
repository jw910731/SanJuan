//
// Created by jw910731 on 2021/6/13.
//

#ifndef SANJUAN_ROOT_GAME_H
#define SANJUAN_ROOT_GAME_H

#include <stdint.h>
#include <stdbool.h>

#define SANJUAN_MAX_CARD 110

/*
 * Card enum for card type representation
 * less than 0 => error stat
 */
enum sanjuan_card{
    /*
     * Invalid state is for null termination or placeholder
     */
    CARD_INVALID = -1,

    CARD_SMITHY = 0,
    CARD_INDIGO_PLANT = 1,
    CARD_SUGAR_MILL = 2,
    CARD_TOBACCO_STORAGE = 3,
    CARD_COFFEE_ROASTER = 4,
    CARD_SILVER_SMELTER = 5,
    CARD_GUILD_HALL = 6,
    CARD_WELL = 7,
    CARD_AQUEDUCT = 8,
    CARD_MARKET_HALL = 9,
    CARD_MARKET_STAND = 10,
    CARD_TRADING_POST = 11,
    CARD_BLACK_MARKET = 12,
    CARD_PREFECTURE = 13,
    CARD_ARCHIVE = 14,
    CARD_CHAPEL = 15,
    CARD_CITY_HALL = 16,
    CARD_CRANE = 17,
    CARD_GOLD_MINE = 18,
    CARD_HERO = 19,
    CARD_STATUE = 20,
    CARD_VICTORY_COLUMN = 21,
    CARD_TRIUMPHAL_ARCH = 22,
    CARD_PALACE = 23,
    CARD_POOR_HOUSE = 24,
    CARD_CARPENTER = 25,
    CARD_QUARRY = 26,
    CARD_TOWER = 27,
    CARD_LIBRARY = 28,
};

/*
 * Card occurrence
 */
static const int32_t init_card_count[] = {
    3, 10, 8, 8, 8, 8, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 2, 2, 3, 3, 3, 3, 3
};

/*
 * Card VP
 * 0 means VP is not static
 */
static const int32_t card_vp[] = {
    1, 1, 1, 2, 2, 3, 0, 1, 2, 2, 1, 1, 1, 2, 1, 2, 0, 1, 1, 5, 3, 4, 0, 0, 1, 2, 2, 2, 3
};

/*
 * Card Cost
 */
static const int32_t card_cost[]={
    1, 1, 2, 3, 4, 5, 6, 2, 3, 4, 2, 2, 2, 3, 1, 3, 6, 2, 1, 5, 3, 4, 6, 6, 2, 3, 4, 3, 5
};

/*
 * Is card city card or production card
 */
static const bool card_isPurple[] = {
    true, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true
};

static const char *sanjuan_rolename[] = {"Builder", "Councillor", "Producer", "Prospector", "Trader"};

enum sanjuan_role{
    /*
     * Invalid role shall be used when user isn't done with role selecting
     */
    ROLE_INVALID = -1,

    ROLE_BUILDER = 0,
    ROLE_COUNCILLOR,
    ROLE_PRODUCER,
    ROLE_PROSPECTOR,
    ROLE_TRADER,
};

/*
 * Client interface struct
 * Note that do not block on input stream for all of the function in client_meta
 * you should let the caller block the input stream
 */
struct sanjuan_client_meta{
    /*
     * ID of this client
     */
    // TODO: Design get id from game component
    int32_t id;

    /*
     * Block fd
     * File descriptor that may block the client
     * caller will use select syscall to prevent blocking
     */
    int fd;

    /*
     * @return for error reporting
     */
    int32_t (*init)();

    /*
     * Choose Role
     * this function should avoid returning invalid role such as selected role
     * @return chosen role or ROLE_INVALID if client is not done with selecting role
     */
    enum sanjuan_role (*choose_role)();

    /*
     * Idle for other blocking operation
     * caller should also check if input stream is available to access or not
     */
    int32_t (*idle)();

    /*
     * Perform Builder
     * @return selected card to build or CARD_INVALID if client is not ready
     */
    enum sanjuan_card (*doBuilder)();
};

/*
 * Game will use global function pointer to represent available method
 * game component is singleton mode.
 */

// Game phase control
extern void (*sanjuan_game_init)(void);
extern void (*sanjuan_game_free)(void);
extern void (*sanjuan_game_start)(void);

// Game fetch API for client

/*
 * Draw card from card supply
 */
extern enum sanjuan_card (*sanjuan_game_draw)();

/*
 * Discard card
 */
extern void (*sanjuan_game_discard)(enum sanjuan_card);

/*
 * Get player count in this game
 */
extern int32_t (*sanjuan_game_player_count)();

/*
 * Query which role is not chosen
 * @return bitmask for which role is currently available, or return 0 for not choosing role
 */
extern int8_t (*sanjuan_game_get_available_role)();
#endif //SANJUAN_ROOT_GAME_H
