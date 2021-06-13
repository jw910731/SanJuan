//
// Created by jw910731 on 2021/6/12.
//

#include "shell.h"

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
    for(const char *it = s ; *it ; ++it){
        if(!isspace(*it)){
            return false;
        }
    }
    return true;
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
            if(strcmp(it, "help") == 0){
                static const char *game_help =  "game command help\n"
                                                "    game start: start a single player game.\n"
                                                "    game connect: connect to a remote server to start game. (not implemented)\n";
                printf("%s", game_help);
            }
            if(strcmp(it, "start") == 0){
                // TODO: setup single player game
            }
            if(strcmp(it, "connect") == 0){
                printf("Current not implemented.\n");
            }
            break;
        }
        case 3:
            return 2;
        default:
            return 0;
    }
    return 1;
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

        // parse command
        int32_t stat = command_parser(line);
        if(stat <= 0){ // error
            printf("Unknown command. Get help message by typing \"help\". \n");
        }
        if(stat > 1){ // exit
            loop_flag = false;
        }

        // save history
        if(empty_string(line)){
            add_history(line);
        }

        // cleanup line
        free(line);
        line = NULL;
    }

    // clean GNU readline
    clear_history ();
}