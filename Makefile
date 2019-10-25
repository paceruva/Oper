main: RiverCrossing.c
	gcc -g -Wall -Wextra -pthread -o main RiverCrossing.c

run: main
	./main