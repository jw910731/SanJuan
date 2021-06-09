set(SOURCE_FILES main.c)

add_executable(SanJuan ${SOURCE_FILES})

target_link_libraries(SanJuan LINK_PUBLIC linenoise)