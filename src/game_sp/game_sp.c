//
// Created by jw910731 on 2021/6/13.
//

#include "game_sp.h"

#include "../game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sanjuan_gamesp_init(char **saveptr){
    char *it = strtok_r(NULL, " ", saveptr);
    if(it == NULL) return 1;
    int fake_count = 0;
    if(strcmp(it, "help") == 0){
        static char *help = "game start help\n"
                            "Usage: game start [AI Type...]\n"
                            "Use at least 1 AI at most 3 AIs to play with you!\n"
                            "Available AI Type:\n"
                            "    Random: AI that do random available action.\n";
        printf("%s", help);
        return 1;
    }
    else{
        ++fake_count;
        for(it = strtok_r(NULL, " ", saveptr) ; it != NULL && fake_count <= 3; it = strtok_r(NULL, " ", saveptr)){
            if(fake_count >= 3) {
                printf("Too much AI player!\n");
                return 1;
            }
            ++fake_count;
        }
    }
    // setup AIs
    sanjuan_game_client_count = fake_count + 1; // add player

    // setup runtime link
    sanjuan_game_free = sanjuan_gamesp_free;
    sanjuan_game_start = sanjuan_gamesp_start;
    return 0;
}

void sanjuan_gamesp_free(){
    printf("Game End (TODO)\n");
}

void sanjuan_gamesp_start(){
    printf("Game Started (WIP)\n");
}