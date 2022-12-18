# Interprocess-Communication
This project functions as an example for interprocess communication where a router gets a requests from a client and sends it to the corresponding workers before receiving the response and printing it

Either run the Makefile to execute the program, or compile it in the following order:


 gcc -Wall -ggdb2 -o router_dealer router_dealer.c -lrt -lm
 
 gcc -Wall -ggdb2 -o client client.c request.c -lrt -lm
 
 gcc -Wall -ggdb2 -o worker_s1 worker_s1.c service1.c -lrt -lm
 
 gcc -Wall -ggdb2 -o worker_s2 worker_s2.c service2.c -lrt -lm
