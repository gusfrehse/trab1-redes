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

    char caminho[100] = {};
    memcpy(caminho, msg.dados, msg.tamanho);

    errno = 0;
    if(chdir(caminho) != 0){
        if(errno == EACCES)
            printf("CD - Erro! Permissão negada\n");
        if(errno == ENOENT)
            printf("CD - Erro! Diretório não encontrado\n");
        return;
    }
    printf("CD OK! nome do diretorio atual: %s\n", caminho);

    // Mudar valores obviamente
    aux.tamanho = 50;
    aux.dados = malloc(50);
    aux.dados = caminho;
    printf("\tMSG A SER ENVIADA(CD)\n");
    imprimirMensagem(aux);
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

            if(recebe.tipo == TIPO_CD)
                executa_cd(recebe);

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
