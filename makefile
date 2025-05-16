CC = gcc
CFLAGS = -Wall -O2

all: treasure_manager treasure_hub calculate_score

prog1: prog1.c
	$(CC) $(CFLAGS) -o tm treasure_manager.c

prog2: prog2.c
	$(CC) $(CFLAGS) -o th treasure_hub.c

prog3: prog3.c
	$(CC) $(CFLAGS) -o cs calculate_score.c

clean:
	rm -f treasure_manager treasure_hub calculate_score
