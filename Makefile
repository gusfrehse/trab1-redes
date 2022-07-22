

all : client server

client : client.c ConexaoRawSocket.o ConexaoRawSocket.h rede.h
server : server.c ConexaoRawSocket.o ConexaoRawSocket.h rede.h
ConexaoRawSocket.o : ConexaoRawSocket.c ConexaoRawSocket.h

.PHONY : clean
clean :
	rm -rf *.o
	rm -rf client
	rm -rf server

