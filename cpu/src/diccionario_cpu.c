#include "diccionario_cpu.h"

// Variables globales CPU
t_log* logger_cpu;
t_config* config;
t_config_cpu config_cpu;

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

// Registro
t_registros* registros_cpu;

//Interrupciones
int interrupcion = 0;
int realizar_desalojo = 0;

uint8_t algoritmo_planificacion;

// Proceso Ejecutando
uint32_t pid_de_cde_ejecutando;

//Nombre instruccion actual ejecutando
t_codigo_instruccion instruccion_actualizada;

// fin variables globales CPU