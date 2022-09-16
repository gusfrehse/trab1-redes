#include "rede.h"
#include "ConexaoRawSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#define TAM_MIN_MSG (16)

#define MSG_BYTE_1(tam, seq, tipo) ((((tam) & 0b111111) << 2) | ((seq) & 0b1100) >> 2)
#define MSG_BYTE_2(tam, seq, tipo) ((((tipo) & 0b111111)) | ((seq) & 0b0011) << 6)
#define MSG_TAM(byte1, byte2) (((byte1) >> 2) & 0b111111)
#define MSG_SEQ(byte1, byte2) ((((byte1) << 2) | ((byte2) >> 6)) & 0b1111)
#define MSG_TIPO(byte1, byte2) (byte2 & 0b111111)

#define TIMEOUT_SEC 2

const msg_info ack = {
    .inicio = MARCADOR_INICIO,
    .tamanho = 0,
    .sequencia = 0,
    .tipo = TIPO_ACK,
    .dados = NULL,
    .paridade = 0,
};

const msg_info nack = {
    .inicio = MARCADOR_INICIO,
    .tamanho = 0,
    .sequencia = 0,
    .tipo = TIPO_NACK,
    .dados = NULL,
    .paridade = 0,
};


int soq;

uint8_t ultimo_tam_seq_tipo = 0;

void iniciaSocket(){
    //joao: enp1s0f0
    //gus1: enp34s0
    //gus2: enp2s0
    soq = ConexaoRawSocket("enp2s0");

    // timeout
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    setsockopt(soq, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

int pegaSocket() {
    return soq;
}

void finalizaSocket(){
    close(soq);
}

int time_out(){
    time_t inicio = time(0);
    time_t tempo_corrido = 0;
    msg_info msg;
    int recebeu_msg = 0;

    while(tempo_corrido < 3 && recebeu_msg != 1){
        msg = receberMensagem();
        if(msg.inicio == MARCADOR_INICIO)
            recebeu_msg = 1;
        tempo_corrido += time(0) - inicio;
    }
    
    if(recebeu_msg)
        return 1;
    return 0;
}

msg_info montaMsg(uint8_t tam, uint8_t seq, uint8_t tipo, uint8_t* dados){
    msg_info msg;

    msg.inicio = MARCADOR_INICIO;
    msg.dados = malloc(sizeof(dados));
    memcpy(msg.dados, dados, tam);
    msg.tamanho = tam;
    msg.tipo = tipo;
    msg.sequencia = seq;
    msg.paridade = calcularParidade(tam, dados);

    return msg;
}

uint8_t calcularParidade(int tam, uint8_t* dados) {
    uint8_t p = 0;

    for (int i = 0; i < tam; i++) {
        p ^= dados[i];
    }

    return p;
}

void mandarMensagem(msg_info info){
    uint8_t msg[68];

    // no máximo 2^4 - 1 = 63 bytes de dados
    info.tamanho = (info.tamanho > TAM_MAX_DADOS) ? TAM_MAX_DADOS : info.tamanho;

    // 1 de inicio + 2 de tam/seq/tipo + n bytes de dados + 1 byte de paridade
    int tam_msg = 1 + 2 + info.tamanho + 1;

    // manda pelo menos TAM_MIN_MSG bytes pra placa de rede ficar feliz
    tam_msg = (tam_msg > TAM_MIN_MSG) ? tam_msg : TAM_MIN_MSG;

    msg[0] = info.inicio;
    msg[1] = MSG_BYTE_1(info.tamanho, info.sequencia, info.tipo);
    msg[2] = MSG_BYTE_2(info.tamanho, info.sequencia, info.tipo);

    // copia os dados para o campo dados da msg
    memcpy(msg + 3, info.dados, info.tamanho);

    // Monta paridade
    msg[3 + info.tamanho] = info.paridade;

    int escrito = write(soq, msg, tam_msg);

    if (escrito <= 0) {
        perror("mandarMensagem(): write()");
    }
}

msg_info receberMensagem() {
    // tamanho max da msg = 8 + 6 + 4 + 6 + 8 * n + 8  = 32 + 8 * n bits = 4 + n bytes
    // como n é representado em 6 bits entao n <= 2^6 = 64 e max == 4 + 64 bytes = 68 bytes
    // tamanho min da msg -> n == 0 -> 32 bits = 4 bytes
    uint8_t msg[68];
    msg_info info = {};

    int lidos = recv(soq, msg, sizeof(msg), 0);

    if (lidos < 0) {
        perror("receberMensagem(): recv()");

        info.inicio = MARCADOR_INICIO;
        info.tipo = TIPO_TIMEOUT;

        return info;
    }

    assert(lidos >= 4); // leu pelo menos o cabeçalho;
    
    info.inicio = msg[0];
    info.tamanho = MSG_TAM(msg[1], msg[2]);
    info.sequencia = MSG_SEQ(msg[1], msg[2]);
    info.tipo = MSG_TIPO(msg[1], msg[2]);

    info.dados = (uint8_t*) malloc(info.tamanho);

    if (!info.dados) {
        perror("receberMensagem(): malloc()");
    }

    memcpy(info.dados, msg + 3, info.tamanho);

    info.paridade = msg[3 + info.tamanho];

    return info;
}

void verificaTipoMensagem(uint8_t tipo){
    switch(tipo){
    case TIPO_CD:
        printf("cd\n"); break;
    case TIPO_OK:
        printf("ok\n"); break;
    case TIPO_NACK:
        printf("nack\n"); break;
    case TIPO_ERRO:
        printf("erro\n"); break;
    case TIPO_LS:
        printf("ls\n"); break;
    case TIPO_LS_MOSTRA_NA_TELA:
        printf("mostra na tela (ls)\n"); break;
    case TIPO_ACK:
        printf("ack\n"); break;
    case TIPO_FIM_TX:
        printf("fim tx\n"); break;
    case TIPO_MKDIR:
        printf("mkdir\n"); break;
    case TIPO_GET:
        printf("get\n"); break;
    case TIPO_DESCRITOR_ARQUIVO:
        printf("descritor arquivo\n"); break;
    case TIPO_DADOS:
        printf("dados\n"); break;
    case TIPO_PUT:
        printf("put\n"); break;
    default:
        printf("tipo nao reconhecido\n"); break;
    }
}

void imprimirMensagem(msg_info msg) {
    printf("\tinicio:    %2x\n", msg.inicio);
    printf("\ttamanho:   %2d\n", msg.tamanho);

    printf("\tsequencia: %2d\n", msg.sequencia);

    printf("\ttipo:      ");

    verificaTipoMensagem(msg.tipo);

    printf("\tdados:     '", msg.tamanho);

    for (int i = 0; i < msg.tamanho; i++) {
        printf("%x ", msg.dados[i]);
    }
    
    printf("' | '");

    for (int i = 0; i < msg.tamanho; i++) {
        printf("%c", msg.dados[i]);
    }

    printf("'\n");

    printf("\tparidade: %2x\n", msg.paridade);
}

void incseq(uint8_t *seq) {
    *seq = (*seq + 1) & 0b1111;
}

