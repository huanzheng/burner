all : client server

client : client.c
	gcc -g -o client client.c -lpthread

server : server.c io.c udp_s.c jobs.c burn.c log.c msg_back_utils.c udp_s.h jobs.h burn.h log.h msg_back_utils.h
	gcc -g -o server server.c io.c udp_s.c jobs.c burn.c log.c msg_back_utils.c -lpthread

clean:
	rm client server
