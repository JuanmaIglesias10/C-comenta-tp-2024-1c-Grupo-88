#include "diccionario_cpu.h"

// Variables globales CPU
t_log* logger_cpu;
t_config* config;
t_config* config_ip;
t_config_cpu config_cpu;

// Colas
t_list* lista_TLB;

// Contador LRU
uint32_t cont_lru;

// File Descriptors 
int fd_memoria;
int fd_cpu_dis;
int fd_cpu_int;
int fd_kernel_dis;
int fd_kernel_int;

//Semaforos

pthread_mutex_t mutex_realizar_desalojo;
pthread_mutex_t mutex_cde_ejecutando;
pthread_mutex_t mutex_instruccion_actualizada;
pthread_mutex_t mutex_interrupcion_consola;

// Registro
t_registros* registros_cpu;

//Interrupciones
int interrupcion = 0;
int realizar_desalojo = 0;
int interrupcion_consola = 0;
uint32_t tam_pagina;

uint8_t algoritmo_planificacion;

// Proceso Ejecutando
uint32_t pid_de_cde_ejecutando;

//Nombre instruccion actual ejecutando
t_codigo_instruccion instruccion_actualizada;

// fin variables globales CPU