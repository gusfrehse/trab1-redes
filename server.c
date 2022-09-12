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
  unsigned int seq_recebimento = 0;
  unsigned int ini, tam, seq, tipo;
  unsigned int lidos = 0;
  iniciaSocketServer();
  iniciaSocketClient();

  printf("---------- Servidor ---------\n");
  for (;;) {
    char *dados;
    receberMensagem(&ini, &tam, &seq, &tipo, &dados, 1);
    if (ini == MARCADOR_INICIO) {
      printf("Recebi mensagem valida!\n");
      printf("01111110 | Tam: %d Seq: %d Tipo: ", tam, seq);
      verifica_tipo_mensagem(tipo);
      printf("Dados:\n");
      for (int i = 0; i < tam; i++) {
          printf("%x ", dados[i]);
      }
      printf("\n");

      //if(seq == seq_recebimento)
      ack();
      //break;
      seq_recebimento++;
      //continue;
    } else {
        printf("Recebi mensagem não válida\n");
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
