cmake_minimum_required(VERSION 3.30)
project(histogram_of_words C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled main.c
        prefix_tree/prefix_tree.h
        prefix_tree/prefix_tree.c
        file_scanner/file_scaner.c
        file_scanner/file_scaner.h)
