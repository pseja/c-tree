CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror
LDFLAGS=-D_DEFAULT_SOURCE

tree: tree.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tree.o: tree.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)