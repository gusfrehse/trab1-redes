#include "rede.h"
#include "ConexaoRawSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>

int soq_client, soq_server;

void iniciaSocketClient(){
    soq_client = ConexaoRawSocket("lo");
}

void iniciaSocketServer(){
    soq_server = ConexaoRawSocket("lo");
}

int pegaSocket() {
    return soq_client;
}

void finalizaSocketClient(){
    close(soq_client);
}

void finalizaSocketServer(){
    close(soq_server);
}

void mandarMensagem(unsigned int tam_dados, unsigned int seq, unsigned int tipo, char* dados, int soq){
    int tamanho_msg = sizeof(cabecalho_mensagem) + tam_dados;
    tamanho_msg = tamanho_msg > 16 ? tamanho_msg : 16; // manda pelo menos 16 bytes pra placa de rede ficar feliz

    cabecalho_mensagem *cab = malloc(tamanho_msg);
    cab->marcador = MARCADOR_INICIO;
    cab->tamanho_seq_tipo = (tam_dados << 10) | (seq << 6) | (tipo);

    memcpy(cab->dados, dados, tam_dados);
    int escrito;
    if(soq == 1)
        escrito = write(soq_server, cab, tamanho_msg);
    else
        escrito = write(soq_client, cab, tamanho_msg);
    //printf("Tam: %d\n", escrito);
}
void ack(){
    mandarMensagem(14, 0, TIPO_ACK, "", 0);
}

void nack(){
    mandarMensagem(14, 0, TIPO_NACK, "", 0);
}

void receberMensagem(unsigned int *ini, unsigned int *tam, unsigned int *seq, unsigned int *tipo, char** dados, int soq) {
    // tamanho max da msg = 8 + 6 + 4 + 6 + 8 * n + 8  = 32 + 8 * n bits = 4 + n bytes
    // como n é representado em 6 bits entao n <= 2^6 = 64 e max == 4 + 64 bytes = 68 bytes
    // tamanho min da msg -> n == 0 -> 32 bits = 4 bytes
    static char buff[68];
    //static char buff[2048];
    int lidos;
    if(soq == 1){
        // Retirar uma leitura caso saia do modo loopback
        lidos = recv(soq_client, buff, sizeof(buff), 0);
        lidos = recv(soq_client, buff, sizeof(buff), 0);
    }
    else{
        lidos = recv(soq_server, buff, sizeof(buff), 0);
        lidos = recv(soq_server, buff, sizeof(buff), 0);
    }

    if (lidos < 0) {
        perror("receberMensagem(): recv()");
        return;
    }

    assert(lidos >= 4); // leu pelo menos o cabeçalho;

    cabecalho_mensagem *msg = (cabecalho_mensagem *) buff;
    *ini = msg->marcador;
    *tam = msg->tamanho_seq_tipo >> 10;
    *seq = msg->tamanho_seq_tipo >> 6 & ((1 << 4) - 1);
    *tipo = msg->tamanho_seq_tipo & ((1 << 6) - 1);
    *dados = buff;
    // TODO: adicionar crc aqui, deve ser algo como *crc = buff[4 + tam];
}

void verifica_tipo_mensagem(unsigned int msg){
    switch(msg){
    case TIPO_CD:
        printf("cd\n"); break;
    case TIPO_OK:
        printf("Ok\n"); break;
    case TIPO_NACK:
        printf("Nack\n"); break;
    case TIPO_ERRO:
        printf("Erro\n"); break;
    case TIPO_LS:
        printf("ls\n"); break;
    case TIPO_LS_MOSTRA_NA_TELA:
        printf("Mostra na tela (ls)\n"); break;
    case TIPO_ACK:
        printf("Ack\n"); break;
    case TIPO_FIM_TX:
        printf("Fim tx\n"); break;
    case TIPO_MKDIR:
        printf("mkdir\n"); break;
    case TIPO_GET:
        printf("get\n"); break;
    case TIPO_DESCRITOR_ARQUIVO:
        printf("Descritor arquivo\n"); break;
    case TIPO_DADOS:
        printf("Dados\n"); break;
    case TIPO_PUT:
        printf("put\n"); break;
    default:
        printf("Tipo nao reconhecido\n"); break;
    }
}
