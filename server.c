#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ConexaoRawSocket.h"
#include "rede.h"

int main() {
    msg_info recebe, envio;
    envio.inicio = MARCADOR_INICIO;

    iniciaSocket();

    printf("---------- Servidor ---------\n");

    for (;;) {

        recebe = receberMensagem();

        if (recebe.inicio == MARCADOR_INICIO) {

            printf("Recebi mensagem ok:\n");
            imprimirMensagem(recebe);

            uint8_t paridade = calcularParidade(recebe.tamanho, recebe.dados);
            if(paridade != recebe.paridade)
              printf("Paridade com erro!\n");
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
