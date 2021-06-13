project(SanJuan)

set(SOURCE_FILES main.c shell.c shell.h)

add_executable(SanJuan ${SOURCE_FILES})

target_link_libraries(SanJuan readline)