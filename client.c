#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ConexaoRawSocket.h"
#include "rede.h"

void ls(char *opcoes) {
    printf("entrando ls\n");
    opcoes[TAM_MAX_DADOS - 1] = '\0'; // limitar string

    uint8_t sequencia = 0;

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

            mandarMensagem(nack);

            free(info.dados);
            continue;
        }

        if (info.sequencia != sequencia) {
            printf("ERRO sequencia ls obtido: %d esperado: %d\n", info.sequencia, sequencia);

            msg_info nseq = nack;
            nseq.sequencia = sequencia;

            mandarMensagem(nseq);

            free(info.dados);
            continue;
        }

        if (info.tipo == TIPO_FIM_TX) {
            printf("fim tx\n");
            free(info.dados);
            break;
        }

        for (int i = 0; i < info.tamanho; i++) {
            putchar(info.dados[i]);
        }
        
        free(info.dados);

        if (info.tipo == TIPO_ERRO) {
            printf("foi um erro\n");
            break;
        }

        msg_info aseq = ack;
        aseq.sequencia = sequencia;
        mandarMensagem(aseq);

        incseq(sequencia);
    }
    printf("saindo ls\n");
}

void cd(char *terminal) {
    printf("entrando cd\n");
    terminal += 3; // ignora o 'cd '

    msg_info info;
    info.inicio = MARCADOR_INICIO;
    info.tamanho = strlen(terminal);
    info.sequencia = 0; // TODO
    info.tipo = TIPO_CD;
    info.dados = terminal;
    info.paridade = calcularParidade(info.tamanho, info.dados);

    msg_info resposta;

start:
    mandarMensagem(info);

    resposta = receberMensagem();

    if (resposta.inicio != MARCADOR_INICIO) {
        printf("inicio errado\n");
        free(resposta.dados);
        goto start;
    }

    if (resposta.tipo == TIPO_OK) {
        printf("OK! Mudado de diretório\n");
    } else if (resposta.tipo == TIPO_ERRO) {
        printf("tipo erro\n");
        for (int i = 0; i < resposta.tamanho; i++) {
            putchar(resposta.dados[i]);
        }

        putchar('\n'); // talvez nao precise
        free(resposta.dados);
    } else if (resposta.tipo == TIPO_NACK) {
        printf("tipo nack\n");
        free(resposta.dados);
        goto start;
    }

    printf("saindo cd\n");
}

void get(char* comando) {

}


int main() {
    iniciaSocket();

    char terminal[100];
    char opcoes[100];

    printf("---------- Terminal Cliente ----------\n");
    printf("$: ");
    fgets(terminal, 98, stdin);
    char *dados;

    msg_info recebe = {};
    msg_info envio = {};
    envio.sequencia = 0; // TODO
    envio.inicio = MARCADOR_INICIO;

    while (strncmp(terminal, "exit", 4)) {
        // Faz o reconhecimento do comando digitado pelo usuário
        if (!strncmp(terminal, "cd", 2)) {
            cd(terminal);
            printf("$: ");
            fgets(terminal, 98, stdin);
            continue;

        } else if (!strcmp(terminal, "mkdir")) {
            // TODO

            scanf("%99s", opcoes);

            envio.tamanho = strlen(opcoes);
            envio.tipo = TIPO_MKDIR;
            envio.dados = opcoes;

        } else if (!strncmp(terminal, "ls", 2)) {
            ls(terminal);
            printf("$: ");
            fgets(terminal, 98, stdin);
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
        fgets(terminal, 98, stdin);
    }

    envio.tamanho = 0;
    envio.tipo = TIPO_FIM_TX;

    mandarMensagem(envio);

    finalizaSocket();
    printf("Fim transmissão\n");
}

// vim: set ts=4 sts=4 sw=4 et:
