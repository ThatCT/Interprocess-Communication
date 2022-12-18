
CC=gcc
CFLAGS=-Wall -ggdb2
LIBS=-lrt -lm

all:  router_dealer client worker_s1 worker_s2

clean:
	rm router_dealer client worker_s1 worker_s2

router_dealer: router_dealer.c settings.h messages.h
	$(CC) $(CFLAGS) -o router_dealer router_dealer.c $(LIBS)

client:  client.c request.c messages.h request.h
	$(CC) $(CFLAGS) -o client client.c request.c $(LIBS)

worker_s1: worker_s1.c service1.c messages.h service1.h
	$(CC) $(CFLAGS) -o worker_s1 worker_s1.c service1.c $(LIBS)

worker_s2: worker_s2.c service2.c messages.h service2.h
	$(CC) $(CFLAGS) -o worker_s2 worker_s2.c service2.c $(LIBS)



