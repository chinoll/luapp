CC := gcc
LUAC := main.o binchunk.o
CFLAGS = -O2 -g
build:$(LUAC)
	$(CC) $(CFLAGS) -o lua++ $(LUAC)
main.o:binchunk.h
binchunk.o:binchunk.h
.PHONY:clean
clean:
	rm $(LUAC) lua++
