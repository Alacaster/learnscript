MAKEFLAGS = --no-builtin-rules
.SUFFIXES:
CFLAGS ?= -Wall -Wextra -ggdb -O0
CC = gcc
objects ?= main.o fuwawinutils.o errorwrapper.o

main.exe: $(objects)
	$(CC) $(CFLAGS) -o $@ $(objects)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: fuwawinutils.h errorwrapper.h
fuwawinutils.o: fuwawinutils.h
errowrapper.o: fuwawinutils.h errorwrapper.h

.PHONY: clean
clean:
	rm *.o

.PHONY: debug release
release: RELEASE = objects='main.o fuwawinutils.o' CFLAGS='-O3 -g0 -s -Wl,--gc-sections -ffunction-sections -fdata-sections'
debug release: clean
	rm main.exe
	$(MAKE) $(RELEASE)