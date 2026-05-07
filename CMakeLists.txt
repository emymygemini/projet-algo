cmake_minimum_required(VERSION 4.0)
project(bosscode C)

set(CMAKE_C_STANDARD 11)

add_executable(bosscode main.c
        boss.h)
