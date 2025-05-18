CC = gcc
CFLAGS = -Wall

all: prog1 prog2 prog3

prog1: treasure_manager.c
	$(CC) $(CFLAGS) -o treasure_manager treasure_manager.c

prog2: treasure_hub.c
	$(CC) $(CFLAGS) -o treasure_hub treasure_hub.c

prog3: calculate_scores.c
	$(CC) $(CFLAGS) -o calculate_scores calculate_scores.c

clean:
	rm -f treasure_manager treasure_hub calculate_scores
