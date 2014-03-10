CROSS=
CC=$(CROSS)gcc
CFLAGS=-O2 -Wall
LIBS=-lpng
INCLUDE=

all:png2fb.o
	$(CC) $(CFLAGS) $(INCLUDE) -o png2fb png2fb.o $(LIBS)
main.o:png2fb.c
	$(CC) $(CFLAGS) $(INCLUDE) -c png2fb.c
clean:
	rm *.o png2fb
