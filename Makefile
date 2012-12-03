# Makefile for Replicated file system
CC := gcc
SERVER_OBJ := server_src/main.o server_src/helper.o server_src/udp.o
MASTER_OBJ := master_src/main.o

#%.o: %.c
#	$(CC) -c -o $@ $< $(LDFLAGS)

#Flags
CFLAGS := -Wall
LDFLAGS := -pthread
LDLIBS :=

default: master server

all: master server

master: $(MASTER_OBJ)
	gcc -o master $(MASTER_OBJ) $(LDFLAGS)

server: $(SERVER_OBJ)
	gcc -o server $(SERVER_OBJ) $(LDFLAGS)

clobber: clean
	-rm -f master server

clean:
	-rm -f core $(MASTER_OBJ) $(SERVER_OBJ)

help:
	@echo ""
	@echo "make		- builds this program"
	@echo "make clean	- remove *.o files"
	@echo "make clobber	- removes all generated files"
	@echo "make help	- this info"
	@echo ""
