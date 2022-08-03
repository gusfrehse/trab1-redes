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
  char* buf = "1234567890123";

  iniciaSocket();
  //mandarMensagem(strlen(buf) + 1, 0, 1, buf);
  //mandarMensagem(strlen(buf) + 1, 0, 3, "0000000000000");
  //mandarMensagem(strlen(buf) + 1, 0, 2, "1111111111111");
  //mandarMensagem(strlen(buf) + 1, 0, 6, "1111111111111");
  //mandarMensagem(strlen(buf) + 1, 0, 7, "9876543210000");
  //mandarMensagem(strlen(buf) + 1, 0, 63, "aaaaaaaaaaaaa");
  //mandarMensagem(strlen(buf) + 1, 0, 8, "facaoterminal");
  //mandarMensagem(strlen(buf) + 1, 0, 9, "getagoraaaaaa");
  //mandarMensagem(strlen(buf) + 1, 0, 46, "             ");
  //mandarMensagem(strlen(buf) + 1, 0, 24, "-------------");
  char terminal[100];
  printf("---------- Terminal Cliente ----------\n");
  printf("$: ");
  scanf("%s", terminal);
  short comando = 0;
  unsigned int sequencia = 0;
  while(strcmp(terminal, "exit")){
    // Faz o reconhecimento do comando digitado pelo usuário
    if(!strcmp(terminal, "ls"))
      mandarMensagem(14, sequencia, 0b000111, "");
    else if(!strcmp(terminal, "cd"))
      mandarMensagem(14, sequencia, 0b000110, "");

    sequencia++;
    printf("$: ");
    scanf("%s", terminal);
  }
  mandarMensagem(14, sequencia, 0b101110, "");
  

  //close(s);
  printf("Fim\n");
}
