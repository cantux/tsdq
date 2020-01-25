
all: tsdq
	gcc test.c tsdq.o -o test -g -I. -lpthread

tsdq:
	gcc -c tsdq.c -o tsdq.o
