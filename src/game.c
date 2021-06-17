//
// Created by jw910731 on 2021/6/13.
//

#include "game.h"

#include <stdint.h>

void (*sanjuan_game_init)(void);
void (*sanjuan_game_free)(void);
void (*sanjuan_game_start)(void);

enum sanjuan_card (*sanjuan_game_draw)();
void (*sanjuan_game_discard)(enum sanjuan_card);
int32_t (*sanjuan_game_player_count)();