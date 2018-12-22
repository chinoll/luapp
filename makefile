CC := gcc
LUAC := main.o lbinchunk.o opcode.o lstack.o lstate.o
CFLAGS = -I include/ -O2 -g
DIR := include
build:$(LUAC)
	$(CC) $(CFLAGS) -o lua++ $(LUAC)
main.o:$(DIR)/lbinchunk.h $(DIR)/lerror.h $(DIR)/opcode.h
lbinchunk.o:$(DIR)/lbinchunk.h $(DIR)/lerror.h $(DIR)/opcode.h
opcode.o:$(DIR)/opcode.h $(DIR)/lerror.h
lstack.o:$(DIR)/lstack.h $(DIR)/lerror.h $(DIR)/lvalue.h $(DIR)/consts.h
lstate.o:$(DIR)/lstate.h $(DIR)/lerror.h
.PHONY:clean
clean:
	rm $(LUAC) lua++
