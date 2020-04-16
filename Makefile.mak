CC=gcc
CFLAGS=-g -Wall

all: shell

shell: shell.c func.o prompt.o loop.o builtin.o func.h
        $(CC) $(CFLAGS) -lreadline shell.c func.o prompt.o loop.o builtin.o -o $@
%o: %c
        $(CC) $(CFLAGS) -c $^

clean:
        rm *.o shell