CC := gcc
LUAC := main.o lbinchunk.o opcode.o lstack.o lstate.o lmath.o convert.o lvm.o inst.o lvalue.o gc.o hashmap.o table.o
CFLAGS = -I include/ -O0 -g -lm -lxxhash -Wall
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
lvalue.o:
list.o:
hashmap.o:
table.o:
gc.o:



.PHONY:clean
clean:
	rm $(LUAC) lua++
