//
// Created by jw910731 on 2021/6/13.
//

#include "game.h"

#include <stdint.h>

#include "client.h"

/*
 * @param char** refers to saveptr in strtok_r
 * @return = 0 refers to game init success (other method is configured correctly)
 *         = 1 refers to game init failed
 */
int (*sanjuan_game_init)(char **);
void (*sanjuan_game_free)(void);
void (*sanjuan_game_start)(void);

/*
 * Clients
 */
struct sanjuan_client_meta sanjuan_game_clients[4];
int32_t sanjuan_game_client_count;