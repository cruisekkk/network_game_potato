CC=gcc
CFLAGS=-O3

all: ringmaster player

ringmaster: ringmaster.c
	$(CC) $(CFLAGS) -o $@ ringmaster.c

player: player.c
	$(CC) $(CFLAGS) -o $@ player.c
