//
// Created by jw910731 on 2021/6/12.
//

#include "shell.h"
#include "game.h"
#include "game_sp/game_sp.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

static const char *help_msg = "SanJuan Shell help message\n"
                        "    help / ? : Print this help message.\n"
                        "    game <option> : To start the game or get game related information.\n"
                        "    exit : Exit shell.\n"
                        "To get detail help message of command. Use <command> -h\n";

static bool empty_string(const char *s){
    bool flag = true;
    for(const char *it = s ; *it ; ++it){
        flag = false;
        if(!isspace(*it)){
            return false;
        }
    }
    return flag;
}

static int32_t command_parser(char *s){
    char *saveptr;
    char *it = strtok_r(s, " ", &saveptr);

    static const char *command_list[] = {"help", "?", "game", "exit"};
    int cmd = -1;
    for(int32_t i  = 0 ; i < (sizeof(command_list) / sizeof(*command_list)) ; ++i){
        if(strcmp(it, command_list[i]) == 0){
            cmd = i;
            break;
        }
    }

    switch(cmd){
        case 0:
        case 1:
            printf("%s", help_msg);
            break;
        case 2: {
            it = strtok_r(NULL, " ", &saveptr);
            if(strcmp(it, "help") == 0){
                static const char *game_help =  "game command help\n"
                                                "    game start: start a single player game.\n"
                                                "    game connect: connect to a remote server to start game. (not implemented)\n";
                printf("%s", game_help);
                break;
            }
            if(strcmp(it, "start") == 0){
                // parse arg and setup runtime link
                if(sanjuan_gamesp_arg_parse(&saveptr)){
                    sanjuan_gamesp_runtime_link();
                    return 3; // game ready to boostrap
                }
            }
            if(strcmp(it, "connect") == 0){
                printf("Currently not implemented.\n");
            }
            return 1; // continue to listen cmd
        }
        case 3:
            return 2;
        default:
            return 0; // error
    }
    return 1; // continue to listen cmd
}

void sanjuan_shell_boostrap(){
    // setup GNU readline
    stifle_history(100);
    rl_bind_key ('\t', rl_insert);

    // mainloop
    char *line = NULL;
    bool loop_flag = true;
    while(loop_flag){
        line = readline("SanJuan sh>");
        if(!line) break;

        if(!empty_string(line)){
            // parse command
            int32_t stat = command_parser(line);
            if(stat <= 0){ // error
                printf("Unknown command. Get help message by typing \"help\". \n");
            }
            if(stat == 2){ // exit
                loop_flag = false;
            }
            if(stat == 3){ // successfully boostrap game
                sanjuan_game_init();
                sanjuan_game_start();
                sanjuan_game_free();
            }

            // save history
            add_history(line);
        }

        // cleanup line
        free(line);
        line = NULL;
    }

    // clean GNU readline
    clear_history ();
}