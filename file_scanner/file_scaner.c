#include "file_scaner.h"

#include <ctype.h>
#include <stdlib.h>

int get_word(FILE *file, char* buffer, size_t buffer_size) {
    int ch;
    do {
        ch = fgetc(file);
        if (ch == EOF) {
            return EOF;
        }
    } while (isspace(ch) || ch == '(');
    ungetc(ch, file);

    char format_string[255];
    sprintf(format_string, "%%%lus", buffer_size);
    return fscanf(file, format_string, buffer);
}

// Проверка, является ли символ началом слова
int is_word_start(char prev, char curr) {
    return (isspace(prev) || prev == '\0' || prev == '(') && !isspace(curr);
}

long* split_file(char* filename, size_t col_parts) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Не удалось открыть файл");
        return NULL;
    }

    // Определяем размер файла
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Вычисляем приблизительный размер каждой части
    long part_size = file_size / col_parts;
    long *part_offsets = malloc((col_parts+1) * sizeof(long));
    if (part_offsets == NULL) {
        perror("Ошибка при выделении памяти");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    part_offsets[0] = 0; // Первая часть всегда начинается с начала файла
    part_offsets[col_parts] = file_size;

    // Определяем начала остальных частей
    for (int i = 1; i < col_parts; i++) {
        fseek(file, i * part_size, SEEK_SET);

        // Определяем начало следующего слова
        fseek(file, -1, SEEK_CUR);
        char prev = fgetc(file);
        char curr;
        while ((curr = fgetc(file)) != EOF) {
            if (is_word_start(prev, curr)) {
                part_offsets[i] = ftell(file) - 1;
                break;
            }
            prev = curr;
        }

        // Если достигли конца файла
        if (feof(file)) {
            part_offsets[i] = file_size;
        }
    }

    fclose(file);
    return part_offsets;
}
