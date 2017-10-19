OBJ  = lib.o main.o
PROG = shm
CC = clang
#CC = gcc
FLAGS = -Wall -std=gnu99
RM = rm -f

$(PROG): $(OBJ)
	$(CC) -o $(PROG) $(FLAGS) $(OBJ)

lib.o: lib.c lib.h config.h
	$(CC) $(FLAGS) -c lib.c

main.o: main.c
	$(CC) $(FLAGS) -c main.c

.PHONY: clean
clean:
	$(RM) *.o $(PROG)
