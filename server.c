//#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "ConexaoRawSocket.h"
#include "rede.h"

void executa_cd(msg_info msg){
    msg_info aux = {};
    aux.inicio = MARCADOR_INICIO;
    aux.tipo = TIPO_DADOS;
    aux.tamanho = 63;
    aux.dados = malloc(63);

    char caminho[100] = {};
    memcpy(caminho, msg.dados, msg.tamanho);

    errno = 0;
    if(chdir(caminho) != 0){
        char *err_str = strerror(errno);
        aux.tamanho = strlen(err_str) + 1;
        memcpy(aux.dados, err_str, aux.tamanho);
        aux.tipo = TIPO_ERRO;
    } else {
        printf("CD OK! nome do diretorio atual: %s\n", caminho);
        memcpy(aux.dados, caminho, msg.tamanho);
    }

    // Mudar valores obviamente
    mandarMensagem(aux);
}

void executa_ls(msg_info msg){
    msg_info aux = {};
    aux.inicio = MARCADOR_INICIO;
    aux.tamanho = 63;
    aux.dados = malloc(63);
    if(aux.dados == NULL){
        printf("Erro no malloc\n");
        exit(1);
    }
    char comando[63] = "ls ";
    strcat(comando, msg.dados);
    printf("Comando: %s\n", comando);

    FILE *arq = popen(comando, "r");
    if(arq == NULL){
        printf("Erro POPEN\n");
        exit(1);
    }

    int lidos;
    while((lidos = fread(aux.dados, 1, 63, arq)) != 0){
        //int lidos = fgets(aux.dados, 63, arq);
        aux.tamanho = lidos;
        aux.inicio = MARCADOR_INICIO;
        aux.tipo = TIPO_DADOS;
        aux.paridade = calcularParidade(aux.tamanho, aux.dados);
        //TODO sequencia
        printf("%s", aux.dados);
        mandarMensagem(aux);
    }
    pclose(arq);
    //free(aux.dados);
    //aux.dados = NULL;
    aux.tamanho = 0;
    aux.inicio = MARCADOR_INICIO;
    aux.tipo = TIPO_FIM_TX;
    aux.paridade = 0;
    //aux.paridade = calcularParidade(aux.tamanho, aux.dados);
    mandarMensagem(aux);
}

int main() {
    msg_info recebe, envio;
    envio.inicio = MARCADOR_INICIO;

    iniciaSocket();

    printf("---------- Servidor ---------\n");

    for (;;) {

        recebe = receberMensagem();
        //recebe = receberMensagem();

        if (recebe.inicio == MARCADOR_INICIO) {

            printf("Recebi mensagem ok:\n");
            imprimirMensagem(recebe);

            uint8_t paridade = calcularParidade(recebe.tamanho, recebe.dados);
            if(paridade != recebe.paridade)
              printf("Paridade com erro!\n");

            if(recebe.tipo == TIPO_CD) {
                executa_cd(recebe);
                continue;
            }
            else if(recebe.tipo == TIPO_LS){
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
