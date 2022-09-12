#ifndef REDE_H
#define REDE_H

#include <stdint.h>

#define MARCADOR_INICIO        (0b01111110)

#define TIPO_CD                (0b00000110)
#define TIPO_OK                (0b00000001)
#define TIPO_NACK              (0b00000010)
#define TIPO_ERRO              (0b00010001)
#define TIPO_LS                (0b00000111)
#define TIPO_LS_MOSTRA_NA_TELA (0b00111111)
#define TIPO_ACK               (0b00000011)
#define TIPO_FIM_TX            (0b00101110)
#define TIPO_MKDIR             (0b00001000)
#define TIPO_GET               (0b00001001)
#define TIPO_DESCRITOR_ARQUIVO (0b00011000)
#define TIPO_DADOS             (0b00100000)
#define TIPO_PUT               (0b00001010)

#define MSG_TAM(x) (((x).tamanho_seq_tipo >> 10) & 0b111111)
#define MSG_SEQ(x) (((x).tamanho_seq_tipo >> 6)  & 0b1111)
#define MSG_TIPO(x) ((x).tamanho_seq_tipo        & 0b111111)

typedef struct cabecalho_mensagem {
  uint8_t marcador;
  uint16_t  tamanho_seq_tipo; // talvez seja 8 bits
  uint8_t dados[];
} cabecalho_mensagem;

typedef struct fim_mensagem {
  uint8_t paridade;
} fim_mensagem;

void mandarMensagem(unsigned int tam_dados, unsigned int seq, unsigned int tipo, char* dados, int soq);
void receberMensagem(unsigned int *ini, unsigned int *tam, unsigned int *seq, unsigned int *tipo, char** dados, int soq);
void iniciaSocketClient();
void iniciaSocketServer();
int pegaSocket();
void finalizaSocketClient();
void finalizaSocketServer();
void verifica_tipo_mensagem(unsigned int msg);
void ack();
void nack();

#endif
