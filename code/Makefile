CFLAGS=-Wall -Wextra -pedantic -Wno-long-long -O2
DFLAGS=-Wall -Wextra -pedantic -Wno-long-long
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
EXEC=cartas
DBUG=$(EXEC).dbg
PRE=$(EXEC).p
CC=gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

clean:
	rm $(EXEC) $(DBUG) *.o *.s $(PRE)

asm:
	$(CC) $(DFLAGS) -S *.c

pre:
	$(CC) $(DFLAGS) -E -o $(PRE) *.c

debug: $(OBJS)
	$(CC) $(DFLAGS) -o $(DBUG) *.c

install: cartas
	sudo cp $(EXEC) /usr/lib/cgi-bin
