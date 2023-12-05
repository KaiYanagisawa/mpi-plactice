CC=gcc
CFLAGS=-Wall -O3

all: sw-sample

sw-sample: sw-sample.o
	$(CC) $(CFLAGS) -o sw-sample sw-sample.o

clean:
	rm -f sw-sample *.o
