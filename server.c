#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>

#include "ConexaoRawSocket.h"
#include "rede.h"
#include "comandos.h"

int main() {
    msg_info recebe, envio;
    envio.inicio = MARCADOR_INICIO;

    iniciaSocket();

    printf("---------- Servidor ---------\n");

    for (;;) {

        usleep(500000);
        printf("esperando msg..\n");
        recebe = receberMensagem();

        if (recebe.inicio == MARCADOR_INICIO) {

            if (recebe.tipo == TIPO_TIMEOUT)
                continue;

            printf("main: Recebi mensagem ok:\n");
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

            if (recebe.tipo == TIPO_GET) {
                executa_get(recebe);
            }

            if (recebe.tipo == TIPO_MKDIR){
                executa_mkdir(recebe);
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

            printf("Recebi algo em que o marcador de início não bate:\n");
            imprimirMensagem(recebe);

        }

        free(recebe.dados);

    }

    finalizaSocket();
    printf("Fim transmissao\n");
}

// vim: set ts=4 sts=4 sw=4 et:
