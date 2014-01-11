CC=gcc
CFLAG=-Wall -Wextra

all: txt.o db.o csv.o
	$(CC) $(CFLAG) -o txt txt.o db.o csv.o

txt.o:
	$(CC) $(CFLAG) -c -o txt.o txt.c

db.o:
	$(CC) $(CFLAG) -c -o db.o db.c

csv.o:
	$(CC) $(CFLAG) -c -o csv.o csv.c

clean:
	rm *.o txt

install:
	cp txt /usr/bin/txt
