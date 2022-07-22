#include "rede.h"
#include "ConexaoRawSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int soq;

void iniciaSocket(){
    soq = ConexaoRawSocket("lo");
}

void mandarMensagem(unsigned int tam, unsigned int seq, unsigned int tipo, char* dados){
    char marcador[14] = {0b01111110};
    write(soq, marcador, sizeof(marcador));
    write(soq, &tam, 1);
    write(soq, &seq, 1);
    write(soq, &tipo, 1);
    write(soq, dados, sizeof(dados));
}