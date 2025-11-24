CC = gcc
CFLAGS = -fdiagnostics-color=always -g

SRC_DIR = src
BIN_DIR = bin

all: okcc


okcc:
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC_DIR)/okcc.c -o $(BIN_DIR)/okcc.exe
