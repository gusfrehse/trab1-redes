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
  uint8_t buf[2048];
  unsigned int ini, tam, seq, tipo;
  unsigned int lidos = 0;
  int acki = 1;
  iniciaSocketServer();
  iniciaSocketClient();

  printf("---------- Servidor ---------\n");
  for (;;) {
    char *dados;
    receberMensagem(&ini, &tam, &seq, &tipo, &dados, 1);
    if (ini == MARCADOR_INICIO) {
      printf("Recebi mensagem valida!\n");
      printf("01111110 | %d %d %d | %s\n", tam, seq, tipo, (char*)dados);
      printf("Tipo de msg: ");
      verifica_tipo_mensagem(tipo);
      //ack();
      //break;
      //seq++;
      //continue;
    }
    //else if(ini != MARCADOR_INICIO)
      //nack();
    if(tipo == TIPO_FIM_TX)
      break;

    //printf("recebi: 0x%x\n", msg.tamanho_seq_tipo);
  }
  
  finalizaSocketServer();
  finalizaSocketClient();
  printf("Fim transmissao\n");

}
