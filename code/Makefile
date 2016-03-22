CFLAGS=-Wall -Wextra -pedantic -Wno-long-long -O2
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
EXEC=cartas
CC=gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

clean:
	rm $(EXEC) *.o

install: cartas
	sudo cp $(EXEC) /usr/lib/cgi-bin
