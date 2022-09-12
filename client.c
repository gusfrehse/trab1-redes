#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ConexaoRawSocket.h"
#include "rede.h"

int main() {
  //printf("tam cabecalho: %lu\n", sizeof(cabecalho_mensagem));
  //return 0;


  //int s = ConexaoRawSocket("lo");
  char marcador[14] = {0b01111110};
  char* buf = "123456789012345678901234";

  iniciaSocketClient();
  iniciaSocketServer();

  char terminal[100];
  char opcoes[100];

  printf("---------- Terminal Cliente ----------\n");
  printf("$: ");
  scanf("%99s", terminal);
  unsigned int ini, tam, seq, tipo;
  short comando = 0;
  unsigned int sequencia = 0;
  char *dados;
  while(strcmp(terminal, "exit")){
    // Faz o reconhecimento do comando digitado pelo usuário
    if(!strcmp(terminal, "cd")){
      scanf("%99s", opcoes);
      mandarMensagem(strlen(opcoes), sequencia, TIPO_CD, opcoes, 1);
    }
    else if(!strcmp(terminal, "mkdir")){
      scanf("%99s", opcoes);
      mandarMensagem(strlen(opcoes), sequencia, TIPO_MKDIR, opcoes, 1);
    }
    else if(!strcmp(terminal, "ls"))
      mandarMensagem(0, sequencia, TIPO_LS, "", 1);
    else if(!strcmp(terminal, "get"))
      mandarMensagem(0, sequencia, TIPO_GET, "", 1);
    else if(!strcmp(terminal, "put"))
      mandarMensagem(0, sequencia, TIPO_PUT, "", 1);
    else{
      printf("%s: command not found\n", terminal);
      sequencia--;
    }
    //printf("recebe agr\n");
    receberMensagem(&ini, &tam, &seq, &tipo, &dados, 0);
    if(ini == MARCADOR_INICIO && tipo == TIPO_ACK)
      printf("Recebi Ack!\n");
    else if(ini == MARCADOR_INICIO && tipo == TIPO_NACK)
      printf("Recebi Nack!\n");
    else {
      printf("Recebi Outra Coisa!\n");
    }
      
    sequencia++;
    printf("$: ");
    scanf("%99s", terminal);
  }
  //ack();
  //nack();
  mandarMensagem(0, sequencia, TIPO_FIM_TX, "", 1);
  finalizaSocketClient();
  finalizaSocketServer();
  printf("Fim transmissao\n");
}
