#ifndef KERNEL_H_
#define KERNEL_H_

#include <diccionario_kernel.h>
#include "consola.h"
#include <atender_kernel.h>
#include <utils.h>


// CONEXIONES
void inicializar_kernel();
void inicializar_listas_colas();
void inicializar_conexiones();
void iniciar_config_kernel();
void inicializar_semaforos();
void inicializar_planificadores();
void inicializar_largo_plazo();
void inicializar_corto_plazo();

// CONSOLA
void iniciar_consola();
void mostrar_opciones_consola();
void atender_consola(char* inpt);
t_codigo_operacion get_codigo_operacion(char* , int );

t_list* ejecutar_script(char* pathScript);
t_list* obtener_lista_script(char* pathScript);
t_codigo_operacion obtener_codigo_operacion(char* parametro);
void iniciar_proceso(char* path);
t_pcb* crear_PCB();
t_pcb* retirar_pcb_de_ready_segun_algoritmo();
t_pcb* retirar_pcb_de(t_queue* cola, pthread_mutex_t* mutex);
void agregar_pcb_a(t_queue* cola, t_pcb* pcb_a_agregar, pthread_mutex_t* mutex);

// Planificacion a largo plazo
void new_a_ready();
void exec_a_finished();


// Planificacion a corto plazo

void ready_a_exec();
char* obtener_elementos_cargados_en(t_queue* );
void enviar_cde_a_cpu();
void recibir_cde_de_cpu();
void evaluar_instruccion(t_instruccion* instruccion_actual);

void agregar_a_cola_finished(char* razon);



#endif