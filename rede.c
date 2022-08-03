#include "rede.h"
#include "ConexaoRawSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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
    cab->marcador = MARCADOR_INICIO;
    cab->tamanho_seq_tipo = (tam_dados << 10) | (seq << 6) | (tipo);

    memcpy(cab->dados, dados, tam_dados);

    int escrito = write(soq, cab, sizeof(cabecalho_mensagem) + tam_dados);
    //printf("Tam: %d\n", escrito);
}
void ack(){
    mandarMensagem(14, 0, TIPO_ACK, "");
}

void nack(){
    mandarMensagem(14, 0, TIPO_NACK, "");
}

void receberMensagem(unsigned int *ini, unsigned int *tam, unsigned int *seq, unsigned int *tipo, char** dados) {
  // tamanho max da msg = 8 + 6 + 4 + 6 + n + 8 = 32 + n = 32 + 64
  // como n é representado em 6 bits entao n <= 2^6 = 64
  // tamanho min da msg -> n == 0 -> 32
  printf("DEBUG: recebendo msg\n");
  static char buff[32 + 64];
  for (;;) {
    printf("DEBUG: lendo socket\n");
    int lidos = recv(soq, buff, sizeof(buff), 0);
    if (lidos < 32) {
      continue;
    }

    printf("DEBUG: li mensagem\n");
    cabecalho_mensagem *msg = (cabecalho_mensagem *) buff;
    *ini = msg->marcador;
    *tam = msg->tamanho_seq_tipo >> 10;
    *seq = msg->tamanho_seq_tipo >> 6 & ((1 << 6) - 1);
    *tipo = msg->tamanho_seq_tipo & ((1 << 8) - 1);
    *dados = buff;

    break;
  }

  printf("DEBUG: mensagem lida\n");
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
