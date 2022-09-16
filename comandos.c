#include <stdlib.h>
#include <stdio.h>
#include "comandos.h"

void executa_get(msg_info msg) {
    printf("entrando get\n");
    msg_info aux = {};
    msg_info descritor_ok = {};
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
    descritor.dados = malloc(4);
    descritor.dados[0] = arq_tam & 0b11111111;
    descritor.dados[1] = (arq_tam >> 8) & 0b11111111;
    descritor.dados[2] = (arq_tam >> 16) & 0b11111111;
    descritor.dados[3] = (arq_tam >> 24) & 0b11111111;
    descritor.paridade = calcularParidade(descritor.tamanho, descritor.dados);

mandar_descritor:
    mandarMensagem(descritor);

receber_descritor_ok:
    descritor_ok = receberMensagem();

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
        printf("mandando %d bytes\n", lidos);
        msg_info resposta;

        aux.tamanho = lidos;
        aux.inicio = MARCADOR_INICIO;
        aux.tipo = TIPO_DADOS;
        aux.paridade = calcularParidade(aux.tamanho, aux.dados);
        aux.sequencia = sequencia;
        printf("%10s", aux.dados); // TODO remover pode dar segfault

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

    printf("dando cd para '%s'\n", caminho);

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

void executa_mkdir(msg_info msg){
    msg_info aux = {};
    aux.inicio = MARCADOR_INICIO;
    aux.tamanho = TAM_MAX_DADOS;
    aux.dados = malloc(TAM_MAX_DADOS);

    char nome_dir[100];
    strcpy(nome_dir, msg.dados);

    if(mkdir(nome_dir, 0755) != 0){
        char *err_str = strerror(errno);

        aux.tipo = TIPO_ERRO;
        aux.tamanho = strlen(err_str) + 1;
        aux.dados = err_str;
        aux.paridade = calcularParidade(aux.tamanho, aux.dados);
        printf("Erro ao criar diretório!");
    }
    else {
        aux.tipo = TIPO_DADOS;
        aux.dados = nome_dir;
        aux.paridade = calcularParidade(aux.tamanho, aux.dados);
        printf("Diretório %s criado\n", nome_dir);
    }
    mandarMensagem(aux);
}

void get(char *comando) {
    printf("entrando get\n");

    comando += 4; // consome "get "
    comando[TAM_MAX_DADOS - 1] = '\0'; // limitar string
    comando[strcspn(comando, "\n")] = '\0';
    msg_info resposta = {};
    msg_info info;
    info.inicio = MARCADOR_INICIO;
    info.tamanho = strlen(comando) + 1;
    info.sequencia = 0;
    info.tipo = TIPO_GET;
    info.dados = comando;
    info.paridade = calcularParidade(info.tamanho, info.dados);

remandar_comando:
    mandarMensagem(info);

resposta_comando:
    resposta = receberMensagem();

    if (resposta.inicio != MARCADOR_INICIO) {
        free(resposta.dados);
        goto resposta_comando;
    }

    if (resposta.tipo == TIPO_TIMEOUT) {
        free(resposta.dados);
        goto remandar_comando;
    }

    if (resposta.paridade != calcularParidade(resposta.tamanho, resposta.dados)) {
        free(resposta.dados);
        
        mandarMensagem(nack);

        goto resposta_comando;
    }

    if (resposta.tipo == TIPO_NACK) {
        free(resposta.dados);
        goto remandar_comando;
    }

    if (resposta.tipo == TIPO_ERRO) {
        for (int i = 0; i < resposta.tamanho; i++) {
            putchar(resposta.dados[i]);
        }
        putchar('\n');

        free(resposta.dados);
        return;
    }

    // ok!
    printf("resposta, é para ser descritor com nome do arquivo!\n");
    imprimirMensagem(resposta);
    assert(resposta.tipo == TIPO_DESCRITOR_ARQUIVO);

    uint32_t tamanho_arq = *((uint32_t *) resposta.dados);

    printf("arquivo é de tamanho %d\n", tamanho_arq);

    int pos = 0;
    uint8_t *buffer = calloc(tamanho_arq, sizeof(uint8_t));

    if (!buffer) {
        perror("get: malloc");
        exit(1);
    }

    free(resposta.dados);

    mandarMensagem(ack);

    uint8_t sequencia = 0;

    while (1) {
        info = receberMensagem();

        if (info.inicio != MARCADOR_INICIO) {
            printf("ERRO marcador inicio get\n");
            free(info.dados);
            continue;
        }

        if (info.paridade != calcularParidade(info.tamanho, info.dados)) {
            printf("ERRO paridade get\n");

            mandarMensagem(nack);

            free(info.dados);
            continue;
        }

        if (info.tipo == TIPO_FIM_TX) {
            printf("fim tx\n");
            free(info.dados);
            break;
        }

        if (info.sequencia != sequencia) {
            printf("ERRO sequencia get obtido: %d esperado: %d\n", info.sequencia, sequencia);

            msg_info nseq = nack;
            nseq.sequencia = sequencia;

            mandarMensagem(nseq);

            free(info.dados);
            continue;
        }

        for (int i = 0; i < info.tamanho; i++) {
            buffer[pos++] = info.dados[i];
            putchar(info.dados[i]);
        }
        putchar('\n');
        
        free(info.dados);

        if (info.tipo == TIPO_ERRO) {
            printf("foi um erro\n");
            break;
        }

        msg_info aseq = ack;
        aseq.sequencia = sequencia;
        mandarMensagem(aseq);

        incseq(&sequencia);
    }

    FILE *outFile = fopen(comando, "w");
    if (!outFile) {
        perror("erro ao criar arquivo local");
        return;
    }

    fwrite(buffer, 1, tamanho_arq, outFile);

    printf("saindo get\n");
}

void ls(char *comando) {
    printf("entrando ls\n");
    comando[TAM_MAX_DADOS - 1] = '\0'; // limitar string
    comando[strcspn(comando, "\n")] = '\0';

    uint8_t sequencia = 0;

    msg_info resposta = {};
    msg_info info;
    info.inicio = MARCADOR_INICIO;
    info.tamanho = strlen(comando) + 1;
    info.sequencia = 0; // TODO
    info.tipo = TIPO_LS;
    info.dados = comando;
    info.paridade = calcularParidade(info.tamanho, info.dados);

remandar_comando:
    mandarMensagem(info);

    while (1) {
        info = receberMensagem();

        if (info.inicio != MARCADOR_INICIO) {
            printf("ERRO marcador inicio ls\n");
            free(info.dados);
            continue;
        }

        if (info.tipo == TIPO_TIMEOUT) {
            free(resposta.dados);
            goto remandar_comando;
        }


        if (info.paridade != calcularParidade(info.tamanho, info.dados)) {
            printf("ERRO paridade ls\n");

            mandarMensagem(nack);

            free(info.dados);
            continue;
        }

        if (info.tipo == TIPO_FIM_TX) {
            printf("fim tx\n");
            free(info.dados);
            break;
        }

        if (info.sequencia != sequencia) {
            printf("ERRO sequencia ls obtido: %d esperado: %d\n", info.sequencia, sequencia);
            imprimirMensagem(info);

            msg_info nseq = nack;
            nseq.sequencia = sequencia;

            mandarMensagem(nseq);

            free(info.dados);
            continue;
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

        incseq(&sequencia);
    }
    printf("saindo ls\n");
}

void cd(char *terminal) {
    printf("entrando cd\n");
    terminal += 3; // ignora o 'cd '
    terminal[strcspn(terminal, "\n")] = '\0';

    msg_info info = {};
    info.inicio = MARCADOR_INICIO;
    info.tamanho = strlen(terminal);
    info.sequencia = 0; // TODO
    info.tipo = TIPO_CD;
    info.dados = terminal;
    info.paridade = calcularParidade(info.tamanho, info.dados);

    msg_info resposta;

start:
    mandarMensagem(info);

resposta_comando:
    resposta = receberMensagem();

    if (resposta.inicio != MARCADOR_INICIO) {
        printf("inicio errado\n");
        free(resposta.dados);
        goto resposta_comando;
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
    } else if (resposta.tipo == TIPO_NACK || resposta.tipo == TIPO_TIMEOUT) {
        printf("tipo nack ou time out\n");
        free(resposta.dados);
        goto start;
    }

    printf("saindo cd\n");
}

void mkdir_client(char *terminal) {
    printf("entrando mkdir\n");
    terminal += 6; // ignora o 'mkdir '
    terminal[strcspn(terminal, "\n")] = '\0';

    msg_info info;
    info.inicio = MARCADOR_INICIO;
    info.tamanho = strlen(terminal);
    info.sequencia = 0; // TODO
    info.tipo = TIPO_MKDIR;
    info.dados = terminal;
    info.paridade = calcularParidade(info.tamanho, info.dados);

    msg_info resposta;

start:
    mandarMensagem(info);

receber_resposta:
    resposta = receberMensagem();

    if (resposta.inicio != MARCADOR_INICIO) {
        printf("inicio errado\n");
        free(resposta.dados);
        goto receber_resposta;
    }
 
    if (resposta.tipo == TIPO_OK) {
        printf("OK! Criado diretório\n");
    } else if (resposta.tipo == TIPO_ERRO) {
        printf("tipo erro\n");
        for (int i = 0; i < resposta.tamanho; i++) {
            putchar(resposta.dados[i]);
        }

        putchar('\n'); // talvez nao precise
        free(resposta.dados);
    } else if (resposta.tipo == TIPO_NACK || resposta.tipo == TIPO_TIMEOUT) {
        printf("tipo nack ou timeout\n");
        free(resposta.dados);
        goto start;
    }

    printf("saindo mkdir\n");
}

void local_ls(char *nome, int tam){
    nome += 1;
    nome[tam - 1] = '\0'; // limitar string
    nome[strcspn(nome, "\n")] = '\0';

    FILE *arq = popen(nome, "r");
    if (arq == NULL){
        printf("Erro POPEN\n");
        exit(1);
    }
    int lidos;
    char *dados = malloc(tam);
    while((lidos = fread(dados, 1, tam, arq)) != 0){
        //printf("mandando %d bytes\n", lidos);
        //msg_info resposta;
        printf("%s", dados);
    }
}

void local_cd(char *nome_dir, int tam){
    nome_dir += 4;
    nome_dir[strcspn(nome_dir, "\n")] = '\0';

    errno = 0;
    if (chdir(nome_dir) != 0) {
        char *err_str = strerror(errno);
        printf("%s\n", err_str);
    } else {
        printf("CD OK! nome do diretorio atual: %s\n", nome_dir);
    }
}

void local_mkdir(char *nome_dir, int tam){
    nome_dir += 7;
    nome_dir[strcspn(nome_dir, "\n")] = '\0';

    errno = 0;
    if(mkdir(nome_dir, 0755) != 0){
        char *err_str = strerror(errno);
        printf("%s\n", err_str);
    }
    else 
        printf("Diretório %s criado\n", nome_dir);
}
