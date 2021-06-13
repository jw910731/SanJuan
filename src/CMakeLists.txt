project(SanJuan)

set(SOURCE_FILES main.c shell.c shell.h game.h game.c game_sp/game_sp.c game_sp/game_sp.h)

add_executable(SanJuan ${SOURCE_FILES})

target_link_libraries(SanJuan readline)