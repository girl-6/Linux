.PHONY:all
all:server client

client:client.c
	gcc -o $@ $^
server:server.c
	gcc -o $@ $^

.PHONY:clean
clean:
	rm -f client server 
