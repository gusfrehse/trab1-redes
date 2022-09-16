#ifndef COMANDOS_H
#define COMANDOS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include "ConexaoRawSocket.h"
#include "rede.h"

// Comandos servidor
void executa_get(msg_info msg);
void executa_ls(msg_info msg);
void executa_cd(msg_info msg);
void executa_mkdir(msg_info msg);

// Comandos cliente
void local_ls(char *comando, int tam);
void local_cd(char *comando, int tam);
void local_mkdir(char *nome_dir, int tam);
void get(char *comando);
void ls(char *comando);
void cd(char *terminal);
void mkdir_client(char *terminal);
void put_client(char *terminal);

#endif 
