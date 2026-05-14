include config.mk

SRC=$(shell find source -name '*.c')
OBJ=$(SRC:.c=.o)

all: debug

debug: CFLAGS+=-g -O0 -DDEBUG
debug: build

release: CFLAGS+=-Os
release: build

source/%.o: source/%.c
	$(CC) -o $@ $(CFLAGS) -c $<

$(OBJ): config.mk

build: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f build $(OBJ)

.PHONY: all debug release clean
