#ifndef DICCIONARIO_MEMORIA_H_
#define DICCIONARIO_MEMORIA_H_

#include <utils.h>

typedef struct{
    int puerto_escucha;
    int tam_memoria;
    int tam_pagina;
    char* path_instrucciones;
    int retardo_respuesta;
 }t_config_memoria;

// variables globales Memoria

// Logger Y Config
extern t_log* logger_memoria;
extern t_config* config;
extern t_config_memoria config_memoria;

// Files Descriptors 
extern int fd_memoria;
extern int fd_cpu;
extern int fd_kernel;
extern int fd_IO;

// fin variables globales Memoria

#endif