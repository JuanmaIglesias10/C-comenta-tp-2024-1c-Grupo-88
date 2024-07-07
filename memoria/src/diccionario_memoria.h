#ifndef DICCIONARIO_MEMORIA_H_
#define DICCIONARIO_MEMORIA_H_

#include <utils.h>
#include "commons/temporal.h"

typedef struct{
    int puerto_escucha;
    int tam_memoria;
    int tam_pagina;
    char* path_instrucciones;
    int retardo_respuesta;
 }t_config_memoria;

typedef struct{
	uint32_t pid;
	t_list* instrucciones;
	uint32_t cantPaginas;
    uint32_t tamaño;
    t_list* listaPaginasProceso;
}t_proceso;

// variables globales Memoria
// Esto representa una fila de la Tabla de Paginas de un Proceso
typedef struct {
	uint32_t pidProcesoCreador;
    uint32_t nroPagina;
    uint32_t nroMarco;
    bool bitModificado;
    bool bitPresencia;
    char* ultimaReferencia; //LRU
    void* direccionFisicaInicio;

    void* direccionLogica;
}t_pagina;


// Logger Y Config
extern t_log* logger_memoria;
extern t_config* config;
extern t_config_memoria config_memoria;

// Files Descriptors 
extern int fd_memoria;
extern int fd_cpu;
extern int fd_kernel;
// extern int fd_IO;

//Listas
extern t_list* listaProcesos;
extern t_list* tablaGlobalPaginas;
extern t_list* listaMarcos;


// Semaforos
extern pthread_mutex_t mutex_lista_procesos;

//Memoria Principal
extern void* memoriaPrincipal;
extern int cantMarcos;

// fin variables globales Memoria




#endif