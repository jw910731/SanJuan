//
// Created by jw910731 on 2021/6/13.
//

#ifndef SANJUAN_ROOT_GAME_H
#define SANJUAN_ROOT_GAME_H

#include <stdint.h>
#include <stdbool.h>

#define SANJUAN_MAX_CARD 110

/** @enum
 * Represent card type
 */
enum sanjuan_card{
    CARD_INVALID = -1, ///< Invalid state is for null termination or placeholder

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

static const char *card_name[] = {"Smithy","Indigo Plant","Sugar Mill","Tobacco Storage","Coffee Roaster","Silver Smelter","Guild Hall","Well","Aqueduct","Market Hall","Market Stand","Trading Post","Black Market","Prefecture","Archive","Chapel","City Hall","Crane","Gold Mine","Hero","Statue","Victory Column","Triumphal Arch","Palace","Poor House","Carpenter","Quarry","Tower","Library"};

static const int32_t init_card_count[] = {
    3, 10, 8, 8, 8, 8, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 2, 2, 3, 3, 3, 3, 3
}; ///< Card occurrence

/** @breif Card VP
 *
 * 0 means VP is not static
 */
static const int32_t card_vp[] = {
    1, 1, 1, 2, 2, 3, 0, 1, 2, 2, 1, 1, 1, 2, 1, 2, 0, 1, 1, 5, 3, 4, 0, 0, 1, 2, 2, 2, 3
};

static const int32_t card_cost[]={
    1, 1, 2, 3, 4, 5, 6, 2, 3, 4, 2, 2, 2, 3, 1, 3, 6, 2, 1, 5, 3, 4, 6, 6, 2, 3, 4, 3, 5
}; ///< Card Cost

static const bool card_isPurple[] = {
    true, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true
}; ///< Is card city card or production card

struct sanjuan_table_card{
    enum sanjuan_card class;
    bool goods;
};

static const char *rolename[] = {"Builder", "Councillor", "Producer", "Prospector", "Trader"};

/** @enum
 * Represent role in San Juan
 */
enum sanjuan_role{
    ROLE_INVALID = -1, ///< Invalid role shall be used when user isn't done with role selecting

    ROLE_BUILDER = 0,
    ROLE_COUNCILLOR,
    ROLE_PRODUCER,
    ROLE_PROSPECTOR,
    ROLE_TRADER,
};

/**
 * Abstract layer for game component to interact with player or AI.
 * Instances of this struct should be treated as an instance of client.
 *
 * @note Do not block on input stream for all of the function in <code>client_meta</code>.
 * The caller should block for you.
 */
struct sanjuan_client_meta{
    int32_t id; ///< ID of this client

    /**
     * Context of this client meta instance
     */
     void *context;

    /**
     * \brief Input blocking fd
     *
     * Input stream file descriptor that may block the client.
     * Caller of all method should use select or epoll syscall to block input this fd.
     *
     * @note Set to -1 if no input stream is used (also refers that no error will returned by the non-blocking method)
     */
    int fd;

    /**
     * Initialize client meta.
     * @p id must initialize prior than this function call
     * @return for error reporting
     */
    int32_t (*init)(struct sanjuan_client_meta *meta);

    /**
     * Free client associated data (especially sanjuan_client_meta::context)
     * @param meta
     */
    void (*free)(struct sanjuan_client_meta *meta);


    /**
     * Choose role hook for client to perform hint text or prepare prompt.
     * @param meta client meta instance
     * @param bitset represent what role is currently available
     */
    void (*pre_choose_role)(struct sanjuan_client_meta *meta, const int8_t bitset);
    /**
     * Actual choose role function
     * this function should avoid returning invalid role such as selected role
     * @param meta client meta instance
     * @return chosen role or ROLE_INVALID if client is not done with selecting role. When success, it self cleanup used resource.
     */
    enum sanjuan_role (*choose_role)(struct sanjuan_client_meta *meta);

    /**
     * Idle for other blocking operation
     * caller should also check if input stream is available to access or not
     * @param meta client meta instance
     * @return preserve for future, currently ignored
     */
    int32_t (*idle)(struct sanjuan_client_meta *meta);

    /**
     * Perform role hook for client to setup and notify which role to perform
     * @param meta client meta instance
     * @param role represent which role to perform
     * @param role_chooser_id the ID of role chooser
     */
    void (*pre_perform_role)(struct sanjuan_client_meta *meta, enum sanjuan_role role, int32_t role_chooser_id);
    /**
     * Actual non-blocking perform role function
     * @param meta client meta instance
     * @return 1 for success and 0 for fail and wait. When success, it self cleanup used resource.
     */
    int32_t (*perform_role)(struct sanjuan_client_meta *meta);

    /**
     * Perform chapel after round
     * @param meta client meta instance
     * @return 1 for success and 0 for fail. No excess cleanup need to be done.
     */
    int32_t (*perform_chapel)(struct sanjuan_client_meta *meta);
};

/*
 * Game will use global function pointer to represent available method
 * game component is singleton mode.
 */

// Game phase control
extern void (*sanjuan_game_init)(void);    ///< Init game component
extern void (*sanjuan_game_free)(void);    ///< Free game related resource
extern void (*sanjuan_game_start)(void);   ///< Start game

// Game fetch API for client

/**
 * Draw card from card supply
 */
extern enum sanjuan_card (*sanjuan_game_draw)();

/**
 * Discard card
 * @param card put this card to discard stack
 */
extern void (*sanjuan_game_discard)(enum sanjuan_card);

/**
 * Get player count in this game
 */
extern int32_t (*sanjuan_game_player_count)();

/**
 * Get player table card status (including production building had goods or not)
 * @param id the player id
 * @return Null terminated card array (Terminated by card class == CARD_INVALID)
 */
extern struct sanjuan_table_card *(*sanjuan_game_get_table)(int32_t id);

/**
 * Get player hand card status
 * @param id player id (client should only ask it's own hand card)
 * @return Null terminated card array (Terminated by CARD_INVALID)
 */
extern enum sanjuan_card *(*sanjuan_game_get_hand)(int32_t id);

/*
 * Player manipulation API
 */

/**
 * Emplace card to player hand card
 * @param id player id
 * @param card card to emplace
 */
extern void (*sanjuan_game_player_emplace_hand)(int32_t id, enum sanjuan_card card);

/**
 * Remove card from player hand card
 * @param id player id
 * @param idx card index in hand card
 */
extern void (*sanjuan_game_player_remove_hand)(int32_t id, int32_t idx);

/**
 * Place hand card on table
 * @param id player id
 * @param card card to place
 * @param idx the index on the table
 *
 * @note this function will not check if place is available, caller should ensure it manually
 */
extern void (*sanjuan_game_player_place_table)(int32_t id, enum sanjuan_card card, int32_t idx);
#endif //SANJUAN_ROOT_GAME_H
