CC = g++
CFLAGS = -std=c++11

all: server client
server: tcp_server.c user.c user.h
	$(CC) $(CFLAGS) tcp_server.c user.c -o server
client: tcp_client.c user.h
	$(CC) $(CFLAGS) tcp_client.c -o client


clean:
	rm -rf *.o
	rm client
	rm server