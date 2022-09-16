

CFLAGS = -g 

all : client server

client : client.c rede.o ConexaoRawSocket.o comandos.o ConexaoRawSocket.h rede.h comandos.h
server : server.c rede.o ConexaoRawSocket.o comandos.o ConexaoRawSocket.h rede.h comandos.h
ConexaoRawSocket.o : ConexaoRawSocket.c ConexaoRawSocket.h
rede.o : rede.h
comandos.o : comandos.c comandos.h

.PHONY : clean
clean :
	rm -rf *.o
	rm -rf client
	rm -rf server

