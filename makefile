CC := gcc
LUAC := main.o lbinchunk.o opcode.o lstack.o lstate.o lmath.o convert.o
CFLAGS = -I include/ -O0 -g -lm
DIR := include
all:$(LUAC)
	$(CC) $(CFLAGS) -o lua++ $(LUAC)
main.o:
lbinchunk.o:
opcode.o:
lstack.o:
lstate.o:
lmath.o:
convert.o:
.PHONY:clean
clean:
	rm $(LUAC) lua++
