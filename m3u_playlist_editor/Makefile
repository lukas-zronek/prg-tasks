# Makefile m3uedit
OBJ  = m3uedit.o read_id3.o lib.o util.o
HEADER = lib.h util.h
INCDIR = .
PROG = m3uedit
CC = cc
FLAGS = -Wall -g -std=c99
RM = rm -f

%.o: %.c
	$(CC) $(FLAGS) -I$(INCDIR) -c $<

$(PROG): $(OBJ)
	$(CC) -o $(PROG) $(FLAGS) $(OBJ) -I$(INCDIR)

.PHONY: clean
clean:
	$(RM) *.o $(PROG)
