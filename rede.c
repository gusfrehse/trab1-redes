#include "rede.h"
#include "ConexaoRawSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>

#define TAMANHO_MINIMO_MSG (16)

int soq_client, soq_server;

uint8_t ultimo_tam_seq_tipo = 0;

void iniciaSocketClient(){
    printf("tamanho struct mensagem: %d\n", sizeof(cabecalho_mensagem));
    soq_client = ConexaoRawSocket("lo");
}

void iniciaSocketServer(){
    soq_server = ConexaoRawSocket("lo");
}

int pegaSocket() {
    return soq_client;
}

void finalizaSocketClient(){
    close(soq_client);
}

void finalizaSocketServer(){
    close(soq_server);
}

void mandarMensagem(unsigned int tam_dados, unsigned int seq, unsigned int tipo, char* dados, int soq){
    // manda pelo menos 16 bytes pra placa de rede ficar feliz
    int tamanho_msg = sizeof(cabecalho_mensagem) + tam_dados + 1;
    tamanho_msg = (tamanho_msg > TAMANHO_MINIMO_MSG) ? tamanho_msg : TAMANHO_MINIMO_MSG;

    uint8_t *buf = malloc(tamanho_msg);

    if (!buf) {
        printf("ERRO malloc\n");
        exit(1);
    }

    cabecalho_mensagem *cab = buf;
    cab->marcador = MARCADOR_INICIO;
    cab->tamanho_seq_tipo = (tam_dados << 10) | (seq << 6) | (tipo);

    // Monta paridade
    uint8_t *paridade = buf + sizeof(cabecalho_mensagem) + tam_dados;
    for(int i = 0;i < tam_dados;i++){
        *paridade ^= dados[i];
    }

    memcpy(cab->dados, dados, tam_dados);
    int escrito;
    if(soq == 1)
        escrito = write(soq_server, cab, tamanho_msg);
    else
        escrito = write(soq_client, cab, tamanho_msg);

    ultimo_tam_seq_tipo = cab->tam_seq_tipo;
    
    //printf("Tam: %d\n", escrito);
}

void ack(){
    //cabecalho_mensagem *cab = malloc(14);
    //cab->marcador = MARCADOR_INICIO;
    //cab->tamanho_seq_tipo = (14 << 10) | (0 << 6) | (TIPO_ACK);
    //write(soq_client, cab, sizeof(cab) + 16);
    //send(soq_client, cab, sizeof(cab) + 16, 0);
    mandarMensagem(0, 0, TIPO_ACK, "", 0);
}

void nack(){
    mandarMensagem(0, 0, TIPO_NACK, "", 0);
}

void receberMensagem(unsigned int *ini, unsigned int *tam, unsigned int *seq, unsigned int *tipo, char** dados, int soq) {
    // tamanho max da msg = 8 + 6 + 4 + 6 + 8 * n + 8  = 32 + 8 * n bits = 4 + n bytes
    // como n é representado em 6 bits entao n <= 2^6 = 64 e max == 4 + 64 bytes = 68 bytes
    // tamanho min da msg -> n == 0 -> 32 bits = 4 bytes
    static char buff[68];
    //static char buff[2048];
    int lidos;
    if(soq == 1){
        // Retirar uma leitura caso saia do modo loopback
        lidos = recv(soq_client, buff, sizeof(buff), 0);
        lidos = recv(soq_client, buff, sizeof(buff), 0);
    }
    else{
        lidos = recv(soq_server, buff, sizeof(buff), 0);
        lidos = recv(soq_server, buff, sizeof(buff), 0);
    }

    if (lidos < 0) {
        perror("receberMensagem(): recv()");
        return;
    }

    uint8_t paridade_msg = buf[sizeof(cabecalho_mensagem) + tam_dados];
    uint8_t paridade = 0;
    for(int i = 0;i < tam_dados;i++){
        *paridade ^= dados[i];
    }

    if (paridade != paridade_msg) {
        printf("ERRO paridade receber mensagem, esperava: %x recebido: %x\n", paridade, paridade_msg);
        // TODO: retornar a paridade em vez de sair direto
        exit(1);
    }

    assert(lidos >= 4); // leu pelo menos o cabeçalho;

    cabecalho_mensagem *msg = (cabecalho_mensagem *) buff;


    *ini = msg->marcador;
    *tam = MSG_TAM(*msg);
    *seq = MSG_SEQ(*msg);
    *tipo = MSG_TIPO(*msg);
    *dados = sizeof(cabecalho_mensagem) + buff;
}

void verifica_tipo_mensagem(unsigned int msg){
    switch(msg){
    case TIPO_CD:
        printf("cd\n"); break;
    case TIPO_OK:
        printf("Ok\n"); break;
    case TIPO_NACK:
        printf("Nack\n"); break;
    case TIPO_ERRO:
        printf("Erro\n"); break;
    case TIPO_LS:
        printf("ls\n"); break;
    case TIPO_LS_MOSTRA_NA_TELA:
        printf("Mostra na tela (ls)\n"); break;
    case TIPO_ACK:
        printf("Ack\n"); break;
    case TIPO_FIM_TX:
        printf("Fim tx\n"); break;
    case TIPO_MKDIR:
        printf("mkdir\n"); break;
    case TIPO_GET:
        printf("get\n"); break;
    case TIPO_DESCRITOR_ARQUIVO:
        printf("Descritor arquivo\n"); break;
    case TIPO_DADOS:
        printf("Dados\n"); break;
    case TIPO_PUT:
        printf("put\n"); break;
    default:
        printf("Tipo nao reconhecido\n"); break;
    }
}
