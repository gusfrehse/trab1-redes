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
  cabecalho_mensagem *msg;
  uint8_t buf[2048];

  int s = ConexaoRawSocket("lo");
  int lidos = 0;
  while ((lidos = read(s, &buf, sizeof(buf)))) {
    printf("li %d bytes\n", lidos);
    msg = (cabecalho_mensagem *) buf;
    if (msg->marcador == 0b01111110) {
      printf("Recebi byte de identificação!\n");
      int tam = msg->tamanho_seq_tipo >> 10;
      int seq = msg->tamanho_seq_tipo >> 6 & ((1 << 5) -1);
      printf("Tam: %d Seq: %d\n", tam, seq);
      printf("dados: %s\n", msg->dados);
    }

    //printf("recebi: 0x%x\n", msg.tamanho_seq_tipo);
  }
  close(s);


  printf("sucesso\n");

}
