SRC = pdb.c compression.c
OBJ = ${SRC:.c=.o}
MOBJ = @$.m.c
CC = gcc
CFLAGS = -g -Wall -lcrypto

all: pdb

pdb: ${OBJ}
	@echo [CC] -o $@
	@${CC} -o $@ ${OBJ} ${LINK} ${CFLAGS}
.c.o:
	@echo [CC] $<
	@${CC} -c ${CFLAGS} $<
clean:
	@echo CLEAN
	@rm -f *.o pdb
