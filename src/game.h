//
// Created by jw910731 on 2021/6/13.
//

#ifndef SANJUAN_ROOT_GAME_H
#define SANJUAN_ROOT_GAME_H

/*
 * Game will use global function pointer to represent available method
 * game component is singleton mode.
 */

extern int (*sanjuan_game_init)(char **);
extern void (*sanjuan_game_free)(void);
extern void (*sanjuan_game_start)(void);

#endif //SANJUAN_ROOT_GAME_H
