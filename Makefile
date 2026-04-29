include config.mk

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: debug

debug: CFLAGS+=-g -O0 -DDEBUG
debug: build

release: CFLAGS+=-Os
release: build

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OBJ): config.mk

build: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f build $(OBJ)

.PHONY: all debug release clean
