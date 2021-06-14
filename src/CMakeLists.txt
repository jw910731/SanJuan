add_subdirectory(util)
add_subdirectory(game_sp)

set(SOURCE_FILES
        main.c
        shell.c
        shell.h
        game.h
        game.c
        $<TARGET_OBJECTS:SanJuan_gameSP>
        $<TARGET_OBJECTS:SanJuan_Util>
)

add_executable(SanJuan ${SOURCE_FILES})

target_link_libraries(SanJuan readline)