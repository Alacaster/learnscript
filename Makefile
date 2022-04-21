CFLAGS ?= -Wall -Wextra -ggdb -Og
CFLAGS += -mtune=intel
CC = gcc
objects ?= errorwrapper.o fuwautils.o main.o consolecontrol.o
.SUFFIXES:
.SUFFIXES: .c .o .h
SHELL = powershell.exe

main.exe: $(objects) Makefile
	$(CC) $(CFLAGS) -o $@ $(objects)

$(objects): %.o : %.c %.h Makefile
main.o errorwrapper.o consolecontrol.o: errorwrapper.h fuwautils.h consolecontrol.h
consolecontrol.o: main.h

.PHONY: clean
clean:
	rm *.o

.PHONY: debug release
release: RELEASE = objects='main.o fuwautils.o consolecontrol.o' CFLAGS='-flto -O3 -g0 -s -Xlinker --gc-sections -ffunction-sections -fdata-sections -D REMOVE_ERROR_WRAPPERS -D NDEBUG'
debug release:
	$(MAKE) $(RELEASE) -B

.PHONY: run runr rund
run: main.exe
	start .\main.exe
runr: release
	start .\main.exe
rund: debug
	gdb main.exe