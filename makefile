TARGET = client
TARGET_PREF = build/

SRC_DIR = .
FILE_SCANNER_DIR = file_scanner
FILE_UTILS_DIR = file_utils
NET_UTILS_DIR = net_utils
PREFIX_TREE_DIR = prefix_tree

CC = gcc
CFLAGS = -std=c11 -I$(FILE_SCANNER_DIR) -I$(PREFIX_TREE_DIR) -I$(FILE_UTILS_DIR) -I$(NET_UTILS_DIR)

# Исходные файлы и объектные файлы
SRCS = $(SRC_DIR)/main.c \
       $(FILE_SCANNER_DIR)/file_scaner.c \
       $(FILE_UTILS_DIR)/file_utils.c \
       $(NET_UTILS_DIR)/net_utils.c \
       $(PREFIX_TREE_DIR)/prefix_tree.c

OBJS = $(SRCS:.c=.o)

all: clean configure rebuild

configure:
	@if ! [ -d ./build/ ]; then \
	mkdir "build"; \
	fi

rebuild:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET_PREF)$(TARGET)

clean:
	rm -rf build/*

help: # Выводит список доступных команд
	@echo "Доступные команды:"
	@echo "  make all        - Полный процесс: очистка, конфигурация и сборка"
	@echo "  make configure  - Создаёт директорию build, если её нет"
	@echo "  make rebuild    - Пересобирает проект"
	@echo "  make clean      - Удаляет все временные файлы из build"