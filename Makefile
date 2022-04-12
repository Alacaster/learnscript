CFLAGS ?= -Wall -Wextra -ggdb -O0
CC = gcc
objects ?= errorwrapper.o fuwawinutils.o main.o

main.exe: $(objects)
	$(CC) $(CFLAGS) -o $@ $(objects)

$(objects): fuwawinutils.h
main.o errorwrapper.o: errorwrapper.h
errorwrapper.o: snippets/switches.c
main.o: main.h

.PHONY: clean
clean:
	rm *.o

.PHONY: debug release
release: RELEASE = objects='main.o fuwawinutils.o' CFLAGS='-O3 -g0 -s -Wl,--gc-sections -ffunction-sections -fdata-sections -D REMOVE_ERROR_WRAPPERS'
debug release:
	$(MAKE) $(RELEASE) -B