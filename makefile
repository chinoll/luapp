CC := gcc
LUAC := main.o lbinchunk.o opcode.o lstack.o lstate.o lmath.o convert.o lvm.o inst.o
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
lvm.o:
inst.o:

.PHONY:clean
clean:
	rm $(LUAC) lua++
