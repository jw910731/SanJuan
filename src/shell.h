//
// Created by jw910731 on 2021/6/12.
//

#ifndef SANJUAN_ROOT_SHELL_H
#define SANJUAN_ROOT_SHELL_H

#include "game.h"

/**
 * Entry point of the program
 * Used to setup and boostrap game component
 */
void sanjuan_shell_boostrap();

/**
 * Template client meta of shell
 */
extern const struct sanjuan_client_meta sanjuan_shell_client_meta;


#endif //SANJUAN_ROOT_SHELL_H
