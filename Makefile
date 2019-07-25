CC=gcc -std=c99
CFLAGS = -D_GNU_SOURCE -ggdb3 -W -Wall -Wextra -Werror -O3
LDFLAGS = 
LIBS = 

default: main

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f main *.o
