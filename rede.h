#ifndef REDE_H
#define REDE_H

#include <stdint.h>
#include <stddef.h>

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
#define TIPO_TIMEOUT           (0b01000000)

#define TAM_MAX_DADOS 63

typedef struct msg_info {
    uint8_t inicio;
    uint8_t tamanho;
    uint8_t sequencia;
    uint8_t tipo;
    uint8_t *dados;
    uint8_t paridade;
} msg_info;

extern const msg_info ack, nack;

void iniciaSocket();
void finalizaSocket();
int pegaSocket();

void mandarMensagem(msg_info info);
msg_info receberMensagem();
msg_info montaMsg(uint8_t tam, uint8_t seq, uint8_t tipo, uint8_t* dados);
void verificaTipoMensagem(uint8_t tipo);
uint8_t calcularParidade(int tam, uint8_t* dados);

void imprimirMensagem(msg_info msg);

void incseq(uint8_t *seq);

#endif
