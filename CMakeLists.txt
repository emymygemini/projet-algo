cmake_minimum_required(VERSION 4.0)
project(projetAlgo2Space C)

set(CMAKE_C_STANDARD 11)


include_directories(D:/allegro/include)
link_directories(D:/allegro/lib)

set(ALLEGRO_DIR "D:/Allegro/allegro")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB DLLS "${ALLEGRO_DIR}/bin/*.dll")
file(COPY ${DLLS} DESTINATION ${CMAKE_BINARY_DIR})

add_executable(projetAlgo2Space main.c
        game.c
        imput.c
        game.c
        fonctions.h
        enemy.h
        Jeu.c
        Jeu.c
        enemy.c
        enemy.c
        enemy.c)

target_link_libraries(projetAlgo2Space
        allegro
        allegro_main
        allegro_primitives
        allegro_image
        allegro_audio
        allegro_acodec)


