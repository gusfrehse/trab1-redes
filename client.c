#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ConexaoRawSocket.h"
#include "rede.h"

void ls(char *opcoes) {
    msg_info info;
    info.inicio = MARCADOR_INICIO;
    info.tamanho = strlen(opcoes) + 1;
    info.sequencia = 0; // TODO
    info.tipo = TIPO_LS;
    info.dados = opcoes;
    info.paridade = calcularParidade(info.tamanho, info.dados);

    mandarMensagem(info);

    while (1) {
        info = receberMensagem();

        if (info.inicio != MARCADOR_INICIO) {
            printf("ERRO marcador inicio ls\n");
            free(info.dados);
            continue;
        }

        if (info.paridade != calcularParidade(info.tamanho, info.dados)) {
            printf("ERRO paridade ls\n");

            msg_info nack;
            info.inicio = MARCADOR_INICIO;
            info.tamanho = 0;
            info.sequencia = 0; // TODO
            info.tipo = TIPO_NACK;
            info.paridade = 0;

            mandarMensagem(nack);

            free(info.dados);
            continue;
        }

        if (info.tipo == TIPO_FIM_TX) {
            free(info.dados);
            break;
        }

        for (int i = 0; i < info.tamanho; i++) {
            putchar(info.dados[i]);
        }
        
        free(info.dados);
    }

}

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
            scanf("%99s", opcoes);
            ls(opcoes);
            printf("$: ");
            scanf("%99s", terminal);
            continue;
        } else if (!strcmp(terminal, "get")) {

            envio.tamanho = 0;
            envio.tipo = TIPO_GET;

        } else if (!strcmp(terminal, "put")) {

            envio.tamanho = 0;
            envio.tipo = TIPO_PUT;

        } else {
            printf("%s: command not found\n", terminal);
        }

        //mandarMensagem(envio);

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
