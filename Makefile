include config.mk

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: build

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OBJ): config.mk

build: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f build $(OBJ) *.dat
