CC = gcc
CFLAGS = -Wall

all: build

build: build_server build_client

build_server:
	$(CC) $(CFLAGS) server.c -o server

build_client:
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f server client