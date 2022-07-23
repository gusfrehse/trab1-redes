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
  printf("tam cabecalho: %lu\n", sizeof(cabecalho_mensagem));
  //return 0;


  //int s = ConexaoRawSocket("lo");
  char marcador[14] = {0b01111110};
  char* buf = "1234567890123";

  iniciaSocket();
  mandarMensagem(strlen(buf) + 1, 0, 0, buf);
  

  //close(s);
  printf("mandei\n");
}
