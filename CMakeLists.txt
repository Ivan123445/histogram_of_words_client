cmake_minimum_required(VERSION 3.30)
project(histogram_of_words_client C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled main.c
        file_utils/file_utils.h
        file_utils/file_utils.c
        prefix_tree/prefix_tree.h
        prefix_tree/prefix_tree.c
        file_scanner/file_scaner.c
        file_scanner/file_scaner.h)
