#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ConexaoRawSocket.h"
#include "rede.h"

int main() {
    iniciaSocket();

    char terminal[100];
    char opcoes[100];

    printf("---------- Terminal Cliente ----------\n");
    printf("$: ");
    scanf("%99s", terminal);
    unsigned int ini, tam, seq, tipo;
    short comando = 0;
    char *dados;
    

    msg_info recebe = {};
    msg_info envio = {};
    envio.sequencia = 0; // TODO
    envio.inicio = MARCADOR_INICIO;

    while (strcmp(terminal, "exit")) {
        // Faz o reconhecimento do comando digitado pelo usuário
        if (!strcmp(terminal, "cd")) {

            scanf("%99s", opcoes);

            envio.tamanho = strlen(opcoes);
            envio.tipo = TIPO_CD;
            envio.dados = opcoes;

        } else if (!strcmp(terminal, "mkdir")) {

            scanf("%99s", opcoes);

            envio.tamanho = strlen(opcoes);
            envio.tipo = TIPO_MKDIR;
            envio.dados = opcoes;

        } else if (!strcmp(terminal, "ls")) {

            envio.tamanho = 0;
            envio.tipo = TIPO_LS;

        } else if (!strcmp(terminal, "get")) {

            envio.tamanho = 0;
            envio.tipo = TIPO_GET;

        } else if (!strcmp(terminal, "put")) {

            envio.tamanho = 0;
            envio.tipo = TIPO_PUT;

        } else {
            printf("%s: command not found\n", terminal);
        }

        mandarMensagem(envio);

        recebe = receberMensagem(); // ack

        if (recebe.inicio == MARCADOR_INICIO) {
            if (recebe.tipo == TIPO_ACK) {
                printf("Recebi Ack!\n");
            } else if (recebe.tipo == TIPO_NACK) {
                printf("Recebi Nack!\n");
            } else {
                printf("Recebi Outra Coisa: \n");
                imprimirMensagem(recebe);
                printf("\n");
            }
        } else {
            //printf("Recebi algo em que o marcador de início não bate\n");
        }

        free(recebe.dados);
            
        printf("$: ");
        scanf("%99s", terminal);
    }

    envio.tamanho = 0;
    envio.tipo = TIPO_FIM_TX;

    mandarMensagem(envio);

    finalizaSocket();
    printf("Fim transmissão\n");
}

// vim: set ts=4 sts=4 sw=4 et:
