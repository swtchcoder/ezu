include config.mk

SRC=ini.c beatmap.c chart.c osu.c main.c
OBJ=$(SRC:.c=.o)

all: build

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OBJ): config.mk

build: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f build $(OBJ)