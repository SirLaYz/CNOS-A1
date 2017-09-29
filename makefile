relay-server: relay-server.o urs-util.o
	gcc -o relay-server relay-server.o urs-util.o

relay-server.o: relay-server.c urs-util.h
	gcc -c relay-server.c

urs-util.o: urs-util.c urs-util.h
	gcc -c urs-util.c

clean:
	rm -f client relay-server *.o

client-1: 
	gcc -c client.c client1

client-2: 
	gcc -c client.c client2

