relay-server: relay-server.o urs-util.o
	gcc -o relay-server relay-server.o urs-util.o

relay-server.o: relay-server.c urs-util.h
	gcc -c relay-server.c

client: client.o
	gcc -o client client.o

client.o: client.c
	gcc -c client.c

urs-util.o: urs-util.c urs-util.h
	gcc -c urs-util.c

clean:
	rm -f client relay-server *.o