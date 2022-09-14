

CFLAGS = -g 

all : client server

client : client.c rede.o ConexaoRawSocket.o ConexaoRawSocket.h rede.h
server : server.c rede.o ConexaoRawSocket.o ConexaoRawSocket.h rede.h
ConexaoRawSocket.o : ConexaoRawSocket.c ConexaoRawSocket.h
rede.o : rede.h

.PHONY : clean
clean :
	rm -rf *.o
	rm -rf client
	rm -rf server

