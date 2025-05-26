all: client init deinit client_remote server

client: local_client.cpp bank.h
	g++ local_client.cpp -o client

init: init.cpp bank.h
	g++ init.cpp -o init

deinit: deinit.cpp bank.h
	g++ deinit.cpp -o deinit

client_remote: client.cpp
	g++ client.cpp -Icolorprint colorprint/libcolorprint.a -o client_remote

server: server.cpp bank_for_remote.h
	g++ server.cpp -o server

clean:
	rm -f client init deinit client_remote server

