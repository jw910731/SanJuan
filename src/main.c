//
// Created by jw910731 on 2021/6/9.
//
#include <stdio.h>

#include <readline/history.h>

#include "shell.h"

int main(){
    // initialize GNU readline
    using_history();

    // shell main loop entry point
    sanjuan_shell_boostrap();
    return 0;
}

