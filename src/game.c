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

struct sanjuan_table_card *(*sanjuan_game_get_table)(int32_t id);
enum sanjuan_card *(*sanjuan_game_get_hand)(int32_t id);
void (*sanjuan_game_player_emplace_hand)(int32_t id, enum sanjuan_card card);
void (*sanjuan_game_player_remove_hand)(int32_t id, int32_t card_seq);
void (*sanjuan_game_player_place_table)(int32_t id, enum sanjuan_card card, int32_t place);
