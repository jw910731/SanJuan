project(SanJuan)
cmake_minimum_required(VERSION 3.17)

add_subdirectory(src)

find_package(Doxygen
             REQUIRED dot)
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_EXTRACT_ALL YES)
if (DOXYGEN_FOUND)
    message("Doxygen enabled")

    doxygen_add_docs(
            doxygen
            ALL
            COMMENT "Generate man pages"
    )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)