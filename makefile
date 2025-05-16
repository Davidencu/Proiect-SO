CC = gcc
CFLAGS = -Wall

all: prog1 prog2 prog3

prog1: treasure_manager.c
	$(CC) $(CFLAGS) -o tm treasure_manager.c

prog2: treasure_hub.c
	$(CC) $(CFLAGS) -o th treasure_hub.c

prog3: calculate_score.c
	$(CC) $(CFLAGS) -o cs calculate_score.c

clean:
	rm -f treasure_manager treasure_hub calculate_score
