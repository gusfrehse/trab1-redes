#ifndef REDE_H
#define REDE_H

#include <stdint.h>

struct dados {
  uint8_t tipo;
  uint8_t dados[];
};

typedef struct cabecalho_mensagem {
  unsigned char marcador;
  uint16_t  tamanho_seq_tipo; // talvez seja 8 bits
  uint8_t dados[];
} cabecalho_mensagem;

typedef struct fim_mensagem {
  uint8_t paridade;
} fim_mensagem;

void mandarMensagem(unsigned int tam_dados, unsigned int seq, unsigned int tipo, char* dados);
void iniciaSocket();
void finalizaSocket();
void verifica_tipo_mensagem(unsigned int msg);
void ack();
void nack();



#endif
