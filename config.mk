CC=cc
BASE_CFLAGS=-Wall -Wextra -pedantic -std=c99 -I$(PWD)/include
DEBUG_CFLAGS=-g -O0 -DDEBUG
RELEASE_CFLAGS=-Os
LDFLAGS=-lm -lzip -lSDL3 -lSDL3_ttf
