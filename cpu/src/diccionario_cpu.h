#ifndef DICCIONARIO_CPU_H_
#define DICCIONARIO_CPU_H_

#include <commons/log.h>
#include <commons/config.h> 
#include <utils.h>
#include <commons/collections/queue.h>

typedef struct{
    char* ip_memoria;
    int puerto_memoria;
    int puerto_escucha_dispatch;
    int puerto_escucha_interrupt;
    int cantidad_entradas_tlb;
    char* algoritmo_tlb;
} t_config_cpu;

typedef struct{
    uint32_t pid;
    uint32_t nro_pagina;
    uint32_t nro_marco;
    uint32_t cont_lru;
} t_entrada_tlb;


// Variables globales CPU

// Colas
extern t_list* lista_TLB;

// Contador LRU
extern uint32_t cont_lru;

// Logger Y Config
extern t_log* logger_cpu;
extern t_config* config;
extern t_config_cpu config_cpu;

// Files Descriptors 
extern int fd_memoria;
extern int fd_cpu_dis;
extern int fd_cpu_int;
extern int fd_kernel_dis;
extern int fd_kernel_int;

// Semaforos

extern pthread_mutex_t mutex_realizar_desalojo;
extern pthread_mutex_t mutex_cde_ejecutando;
extern pthread_mutex_t mutex_instruccion_actualizada;

// Registro
extern t_registros* registros_cpu;

//Interrupciones
extern int interrupcion;
extern int realizar_desalojo;
extern uint32_t tam_pagina;

extern uint8_t algoritmo_planificacion;

// Proceso Ejecutando
extern uint32_t pid_de_cde_ejecutando;

//nombre Instruccion actual ejecutando
extern t_codigo_instruccion instruccion_actualizada;

// Fin variables globales CPU

#endif /* DICCIONARIO_CPU_H_ */