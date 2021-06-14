//
// Created by jw910731 on 2021/6/13.
//

#ifndef SANJUAN_ROOT_GAME_H
#define SANJUAN_ROOT_GAME_H

#include "client.h"
#include <stdint.h>

/*
 * Card enum for card type representation
 */
enum sanjuan_card{
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

typedef enum sanjuan_card sanjuan_deck[100];

static const char *sanjuan_rolename[] = {"Builder", "Councillor", "Producer", "Prospector", "Trader"};

enum sanjuan_role{
    ROLE_BUILDER = 0,
    ROLE_COUNCILLOR,
    ROLE_PRODUCER,
    ROLE_PROSPECTOR,
    ROLE_TRADER,
};

/*
 * Clients
 */
extern struct sanjuan_client_meta sanjuan_game_clients[4];
extern int32_t sanjuan_game_client_count;

/*
 * Game will use global function pointer to represent available method
 * game component is singleton mode.
 */

// Game phase control
extern int (*sanjuan_game_init)(char **);
extern void (*sanjuan_game_free)(void);
extern void (*sanjuan_game_start)(void);

#endif //SANJUAN_ROOT_GAME_H
