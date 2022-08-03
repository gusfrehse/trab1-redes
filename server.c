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
  unsigned int ini, tam, seq, tipo;
  int s = ConexaoRawSocket("lo");
  unsigned int lidos = 0;
  printf("---------- Servidor ---------\n");
  //while ((lidos = read(s, &buf, sizeof(buf)))) {
  for (;;) {
    //printf("li %d bytes\n", lidos);
    //msg = (cabecalho_mensagem *) buf;
    char *dados;
    receberMensagem(&ini, &tam, &seq, &tipo, &dados);
    if (msg->marcador == MARCADOR_INICIO) {
      printf("Recebi mensagem valida!\n");
      tam = msg->tamanho_seq_tipo >> 10;
      seq = msg->tamanho_seq_tipo >> 6 & ((1 << 5) -1);
      tipo = msg->tamanho_seq_tipo << 10;
      tipo = tipo >> 10 & 0b0000000000111111;
      printf("01111110 | %d %d %d | %s\n", tam, seq, tipo, msg->dados);
      //printf("Tam: %d Seq: %d Tipo: %d\n", tam, seq, tipo);
      //printf("dados: %s\n", msg->dados);
      printf("Tipo de msg: ");
      verifica_tipo_mensagem(tipo);
    }
    if(tipo == TIPO_FIM_TX)
      break;

    //printf("recebi: 0x%x\n", msg.tamanho_seq_tipo);
  }
  
  close(s);
  printf("Fim transmissao\n");

}
