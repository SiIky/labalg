CFLAGS=-Wall -Wextra -pedantic -Wno-long-long -O2
DFLAGS=-Wall -Wextra -pedantic -Wno-long-long -g
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
EXEC=bigtwo
DBUG=$(EXEC).dbg
PRE=$(EXEC).p
CC=gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

install: $(EXEC)
	sudo cp $(EXEC) /usr/lib/cgi-bin

clean:
	rm $(EXEC) $(DBUG) *.o *.s $(PRE)

debug:
	$(CC) $(DFLAGS) -o $(DBUG) *.c

asm:
	$(CC) $(DFLAGS) -S *.c

pre:
	$(CC) $(DFLAGS) -E -o $(PRE) *.c
