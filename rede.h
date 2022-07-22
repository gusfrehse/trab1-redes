#ifndef REDE_H
#define REDE_H

#include <stdint.h>

struct dados {
  uint8_t tipo;
  uint8_t dados[];
};

typedef struct cabecalho_mensagem {
  uint8_t marcador;
  uint16_t tamanho:6; // talvez seja 8 bits
  uint16_t sequencia:4;
  uint16_t tipo:6; // talvez seja 8 bits
} cabecalho_mensagem;

typedef struct fim_mensagem {
  uint8_t paridade;
} fim_mensagem;

void mandarMensagem(unsigned int tam, unsigned int seq, unsigned int tipo, char* dados);
void iniciaSocket();



#endif
