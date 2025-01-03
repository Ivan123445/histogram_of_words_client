#ifndef FILE_SCANER_H
#define FILE_SCANER_H
#include <stdio.h>

int get_word(FILE *file, char* buffer, size_t buffer_size);
long* split_file(char *filename, size_t col_parts);

#endif //FILE_SCANER_H
