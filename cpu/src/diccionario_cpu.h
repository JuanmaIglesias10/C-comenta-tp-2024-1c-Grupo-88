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

// Variables globales CPU

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

// Registro
extern t_registros* registros_cpu;

// Fin variables globales CPU

#endif /* DICCIONARIO_CPU_H_ */