CC = gcc
CFLAGS = -fdiagnostics-color=always -g -c -Iinclude -Wall

SRC_DIR = src
BIN_DIR = bin

all: always lexer okcc link

always:
	mkdir -p bin

lexer:
	$(CC) $(CFLAGS) $(SRC_DIR)/lexer.c -o $(BIN_DIR)/lexer.o

okcc:
	$(CC) $(CFLAGS) $(SRC_DIR)/okcc.c -o $(BIN_DIR)/okcc.o

link:
	$(CC) $(BIN_DIR)/lexer.o $(BIN_DIR)/okcc.o -o $(BIN_DIR)/okcc.exe
	rm bin/*.o
