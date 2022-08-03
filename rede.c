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

void finalizaSocket(){
    close(soq);
}

void mandarMensagem(unsigned int tam_dados, unsigned int seq, unsigned int tipo, char* dados){
    cabecalho_mensagem *cab = malloc(sizeof(cabecalho_mensagem) + tam_dados);
    cab->marcador = 0b01111110;
    cab->tamanho_seq_tipo = (tam_dados << 10) | (seq << 6) | (tipo);

    memcpy(cab->dados, dados, tam_dados);

    int escrito = write(soq, cab, sizeof(cabecalho_mensagem) + tam_dados);
    //printf("Tam: %d\n", escrito);
}
void ack(){
    mandarMensagem(14, 0, 0b000011, "");
}

void nack(){
    mandarMensagem(14, 0, 0b000010, "");
}

void verifica_tipo_mensagem(unsigned int msg){
    switch(msg){
    case 0b000110:
        printf("cd\n"); break;
    case 0b00001:
        printf("Ok\n"); break;
    case 0b000010:
        printf("Nack\n"); break;
    case 0b10001:
        printf("Erro\n"); break;
    case 0b000111:
        printf("ls\n"); break;
    case 0b111111:
        printf("Mostra na tela (ls)\n"); break;
    case 0b000011:
        printf("Ack\n"); break;
    case 0b101110:
        printf("Fim tx\n"); break;
    case 0b001000:
        printf("mkdir\n"); break;
    case 0b001001:
        printf("get\n"); break;
    case 0b011000:
        printf("Descritor arquivo\n"); break;
    case 0b100000:
        printf("Dados\n"); break;
    case 0b001010:
        printf("put\n"); break;
    default:
        printf("Tipo nao reconhecido\n"); break;
    }
}
