#include "rede.h"
#include "ConexaoRawSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int soq;

void iniciaSocket(){
    soq = ConexaoRawSocket("lo");
}

void mandarMensagem(unsigned int tam_dados, unsigned int seq, unsigned int tipo, char* dados){
    cabecalho_mensagem *cab = malloc(sizeof(cabecalho_mensagem) + tam_dados);
    cab->marcador = 0b01111110;
    cab->tamanho_seq_tipo = (tam_dados << 10) | (seq << 6) | (tipo);

    memcpy(cab->dados, dados, tam_dados);

    int escrito = write(soq, cab, sizeof(cabecalho_mensagem) + tam_dados);
    printf("Tam: %d\n", escrito);
}
