CC := gcc
LUAC := main.o lbinchunk.o
CFLAGS = -I include/ -O2 -g
DIR := include
build:$(LUAC)
	$(CC) $(CFLAGS) -o lua++ $(LUAC)
main.o:$(DIR)/lbinchunk.h $(DIR)/lerror.h
lbinchunk.o:$(DIR)/lbinchunk.h $(DIR)/lerror.h
.PHONY:clean
clean:
	rm $(LUAC) lua++
