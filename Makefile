CC = gcc
CFLAGS = -std=c99 -g
TAR = mspy-crawl mspy-search

all: $(TAR)

.SUFFIXES: .c.o

.c.o:
	$(CC) -c $< $(CFLAGS)

mspy-crawl: partition.o filter.o crawler.o myutil.o 
	$(CC) -o $@ $^
	
mspy-search: search.o spy.o partition.o filter.o myutil.o query.o
	$(CC) -o $@ $^

clean:
	rm *.o -f
	rm $(TAR) -f

