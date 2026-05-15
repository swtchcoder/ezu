include config.mk

SRC=$(shell find source -name '*.c')
OBJ=$(SRC:.c=.o)
CFLAGS=$(BASE_CFLAGS)

all: .clangd debug

.clangd: config.mk
	printf "CompileFlags:\n" > .clangd
	printf "  Add:\n" >> .clangd
	for flag in $(BASE_CFLAGS) $(DEBUG_CFLAGS); do \
		printf "    - %s\n" "$$flag" >> .clangd; \
	done

debug: CFLAGS+=$(DEBUG_CFLAGS)
debug: build

release: CFLAGS+=$(RELEASE_CFLAGS)
release: build

source/%.o: source/%.c
	$(CC) -o $@ $(CFLAGS) -c $<

$(OBJ): config.mk

build: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f build $(OBJ)

.PHONY: all debug release clean
