#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "ConexaoRawSocket.h"
#include "rede.h"

void executa_get(msg_info msg) {
    printf("entrando get\n");
    msg_info aux = {};
    aux.inicio = MARCADOR_INICIO;
    aux.tamanho = TAM_MAX_DADOS;
    aux.dados = malloc(TAM_MAX_DADOS);

    if (aux.dados == NULL){
        printf("Erro no malloc\n");
        exit(1);
    }

    printf("Arquivo: %s\n", msg.dados);

    FILE *arq = fopen(msg.dados, "r");
    if (arq == NULL){
        // erro ao ler arquivo
        char *erro_str = strerror(errno);
        printf("Erro FOPEN\n");

        msg_info erro;
        erro.inicio = MARCADOR_INICIO;
        erro.tipo = TIPO_ERRO;
        erro.sequencia = 0;

        erro.dados = malloc(TAM_MAX_DADOS);
        if (!erro.dados) {
            printf("erro malloc\n");
            exit(1);
        }

        memcpy(erro.dados, erro_str, TAM_MAX_DADOS);
        erro.dados[TAM_MAX_DADOS - 1] = '\0';
        erro.tamanho = strlen(erro.dados);
        erro.paridade = calcularParidade(erro.tamanho, erro.dados);
        mandarMensagem(erro);

        return;
    }

    fseek(arq, 0, SEEK_END);

    uint32_t arq_tam = ftell(arq);

    rewind(arq);
    
    msg_info descritor;
    descritor.inicio = MARCADOR_INICIO;
    descritor.tamanho = 4;
    descritor.sequencia = 0;
    descritor.tipo = TIPO_DESCRITOR_ARQUIVO;
    descritor.dados = (uint8_t *) &arq_tam;
    descritor.paridade = calcularParidade(descritor.tamanho, descritor.dados);

mandar_descritor:
    mandarMensagem(descritor);

receber_descritor_ok:
    msg_info descritor_ok = receberMensagem();

    if (descritor_ok.inicio != MARCADOR_INICIO) {
        free(descritor_ok.dados);
        goto receber_descritor_ok;
    }

    if (descritor_ok.paridade != calcularParidade(descritor_ok.tamanho, descritor_ok.dados)) {
        free(descritor_ok.dados);
        // wtf
        printf("eita");
    }

    if (descritor_ok.tipo == TIPO_NACK) {
        free(descritor_ok.dados);
        goto mandar_descritor;
    }

    assert(descritor_ok.tipo == TIPO_ACK);

    uint8_t sequencia = 0;
    int lidos;
    while((lidos = fread(aux.dados, 1, TAM_MAX_DADOS, arq)) != 0){
        msg_info resposta;

        aux.tamanho = lidos;
        aux.inicio = MARCADOR_INICIO;
        aux.tipo = TIPO_DADOS;
        aux.paridade = calcularParidade(aux.tamanho, aux.dados);
        aux.sequencia = sequencia;
        printf("%s", aux.dados);

remandar:
        mandarMensagem(aux);

receber:
        resposta = receberMensagem();

        if (resposta.inicio != MARCADOR_INICIO) {
            printf("erro marcador inicio resposta\n");
            goto receber;
        }

        if (resposta.tipo == TIPO_NACK) {
            printf("nack resposta\n");
            goto remandar;
        }

        incseq(&sequencia);
    }

    free(aux.dados);

    aux.inicio = MARCADOR_INICIO;
    aux.tamanho = 0;
    aux.sequencia = 0;
    aux.tipo = TIPO_FIM_TX;
    aux.dados = NULL;
    aux.paridade = 0;

    mandarMensagem(aux);
    printf("saindo get\n");
    

}

void executa_ls(msg_info msg) {
    printf("entrando ls\n");
    msg_info aux = {};
    aux.inicio = MARCADOR_INICIO;
    aux.tamanho = TAM_MAX_DADOS;
    aux.dados = malloc(TAM_MAX_DADOS);

    if (aux.dados == NULL){
        printf("Erro no malloc\n");
        exit(1);
    }

    printf("Comando: %s\n", msg.dados);

    FILE *arq = popen(msg.dados, "r");
    if (arq == NULL){
        printf("Erro POPEN\n");
        exit(1);
    }

    printf("vou comecar a mandar dados\n");

    uint8_t sequencia = 0;
    int lidos;
    while((lidos = fread(aux.dados, 1, TAM_MAX_DADOS, arq)) != 0){
        printf("mandando %d bytes\n", lidos);
        msg_info resposta;

        aux.inicio = MARCADOR_INICIO;
        aux.tamanho = lidos;
        aux.sequencia = sequencia;
        aux.tipo = TIPO_DADOS;
        aux.paridade = calcularParidade(aux.tamanho, aux.dados);
        printf("%s", aux.dados);

remandar:
        mandarMensagem(aux);

receber:
        resposta = receberMensagem();

        if (resposta.inicio != MARCADOR_INICIO) {
            printf("erro marcador inicio resposta\n");
            goto receber;
        }

        if (resposta.tipo == TIPO_NACK) {
            printf("nack resposta\n");
            goto remandar;
        }

        incseq(&sequencia);
    }

    printf("terminei de mandar dados\n");

    free(aux.dados);

    aux.inicio = MARCADOR_INICIO;
    aux.tamanho = 0;
    aux.sequencia = 0;
    aux.tipo = TIPO_FIM_TX;
    aux.dados = NULL;
    aux.paridade = 0;

    mandarMensagem(aux);
    printf("saindo ls\n");
}

void executa_cd(msg_info msg){
    msg_info aux = {};
    aux.inicio = MARCADOR_INICIO;
    aux.tipo = TIPO_DADOS;
    aux.tamanho = TAM_MAX_DADOS;
    aux.dados = malloc(TAM_MAX_DADOS);

    char caminho[TAM_MAX_DADOS + 1] = {};
    memcpy(caminho, msg.dados, msg.tamanho);
    caminho[TAM_MAX_DADOS] = '\0';

    errno = 0;
    if (chdir(caminho) != 0) {
        char *err_str = strerror(errno);

        aux.tamanho = strlen(err_str) + 1;
        memcpy(aux.dados, err_str, aux.tamanho);

        aux.tipo = TIPO_ERRO;
    } else {
        printf("CD OK! nome do diretorio atual: %s\n", caminho);

        aux.tamanho = strlen(caminho) + 1;
        memcpy(aux.dados, caminho, msg.tamanho);
    }

    mandarMensagem(aux);
}

int main() {
    msg_info recebe, envio;
    envio.inicio = MARCADOR_INICIO;

    iniciaSocket();

    printf("---------- Servidor ---------\n");

    for (;;) {

        printf("esperando msg..\n");
        recebe = receberMensagem();

        if (recebe.inicio == MARCADOR_INICIO) {

            printf("Recebi mensagem ok:\n");
            imprimirMensagem(recebe);

            if (calcularParidade(recebe.tamanho, recebe.dados) != recebe.paridade) {
                free(recebe.dados);

                mandarMensagem(nack);

                continue;
            }

            if (recebe.tipo == TIPO_CD) {
                executa_cd(recebe);
                continue;
            }

            if (recebe.tipo == TIPO_LS){
                executa_ls(recebe);
                continue;
            }

            if (recebe.tipo == TIPO_FIM_TX)
                break;

            if (recebe.tipo != TIPO_ACK) {
                envio.tamanho = 0;
                envio.tipo = TIPO_ACK;
                mandarMensagem(envio);
            }

        } else {

            //printf("Recebi algo em que o marcador de início não bate:\n");
            //imprimirMensagem(recebe);

        }

        free(recebe.dados);

    }

    finalizaSocket();
    printf("Fim transmissao\n");
}

// vim: set ts=4 sts=4 sw=4 et:
