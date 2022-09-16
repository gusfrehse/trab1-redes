#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "ConexaoRawSocket.h"
#include "rede.h"
#include "comandos.h"

int main() {
    iniciaSocket();

    char terminal[100];

    printf("---------- Terminal Cliente ----------\n");
    printf("$: ");
    fgets(terminal, 98, stdin);
    char *dados;

    msg_info recebe = {};
    msg_info envio = {};
    envio.sequencia = 0;
    envio.inicio = MARCADOR_INICIO;

    while (strncmp(terminal, "exit", 4)) {
        // Faz o reconhecimento do comando digitado pelo usuário
        if (!strncmp(terminal, "cd", 2)) {
            cd(terminal);
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;
        } else if (!strncmp(terminal, "lcd", 3)){
            local_cd(terminal, strlen(terminal));
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;
        } else if (!strncmp(terminal, "ls", 2)) {
            ls(terminal);
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;
        } else if (!strncmp(terminal, "lls", 3)) {
            local_ls(terminal, strlen(terminal));
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;
        } else if (!strncmp(terminal, "mkdir", 5)) {
            mkdir_client(terminal);
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;
        } else if (!strncmp(terminal, "lmkdir", 6)) {
            local_mkdir(terminal, strlen(terminal));
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;
        } else if (!strncmp(terminal, "get", 3)) {
            get(terminal);
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;
        } else if (!strncmp(terminal, "put", 3)) {
            put_client(terminal);
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;
        } else {
            printf("%s: command not found\n", terminal);
        }

        //mandarMensagem(envio);

        printf("$: ");
        fgets(terminal, 98, stdin);
    }

    envio.tamanho = 0;
    envio.tipo = TIPO_FIM_TX;

    mandarMensagem(envio);

    finalizaSocket();
    //printf("Fim transmissão\n");
}

// vim: set ts=4 sts=4 sw=4 et:
