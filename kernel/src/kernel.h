#ifndef KERNEL_H_
#define KERNEL_H_

#include <diccionario_kernel.h>
#include "consola.h"
#include <atender_kernel.h>
#include <utils.h>
#include "commons/temporal.h"
#include "cambio_de_estado.h"
#include "manejoIO.h"

// INIT
void inicializar_kernel(char* nombre_arch_config);
void inicializar_listas_colas();
void inicializar_conexiones();
void iniciar_config_kernel(char* nombre_arch_config);
void inicializar_semaforos();
void inicializar_planificadores();
void inicializar_largo_plazo();
void inicializar_corto_plazo();
t_recurso* inicializar_recurso(char* nombre_recurso, int instancias_totales);


// CONSOLA
void iniciar_consola();
void mostrar_opciones_consola();
void atender_consola(char* inpt);
t_codigo_operacion get_codigo_operacion(char* , int );


// FUNCIONALIDAD CONSOLA
t_list* obtener_lista_script(char* pathScript);
void iniciar_proceso(char* path);
void finalizar_proceso(char* pid_string);
void liberar_recursos_pcb(t_pcb* pcb);
void signal_recursos_asignados_pcb(t_pcb* pcb, char* nombre_recurso_pedido);
void detenerPlanificacion();
void iniciarPlanificacion();
void procesosPorEstado();
t_pcb* crear_PCB();
void cambiar_grado_multiprogramacion(char* nuevo_grado);
void destruir_pcb(t_pcb* pcb);

// FUNCIONES AUX
t_codigo_operacion obtener_codigo_operacion(char* parametro);
t_pcb* retirar_pcb_de_ready_segun_algoritmo();
t_pcb* retirar_pcb_de(t_queue* cola, pthread_mutex_t* mutex);
void retirar_pcb_de_su_respectivo_estado(uint32_t pid, int* resultado);
void enviar_a_finalizado(t_pcb* pcb_a_finalizar, char* razon);
t_pcb* encontrar_pcb_por_pid(uint32_t pid, int* encontrado);
void agregar_pcb_a(t_queue* cola, t_pcb* pcb_a_agregar, pthread_mutex_t* mutex);
char* obtener_elementos_cargados_en(t_queue* );
int esta_proceso_en_cola_bloqueados(t_pcb* pcb);
bool es_RR_o_VRR();
char* obtener_nombre_estado(t_estado_proceso estado);

// INTERACCIONES CON CPU
void enviar_cde_a_cpu();
void recibir_cde_de_cpu();


// RR & VRR
void prender_quantum();
void controlar_tiempo_de_ejecucion();
void timer_vrr();


// EVALUAR INSTRUCCIONES
void evaluar_instruccion(t_instruccion* instruccion_actual);

void evaluar_wait(char* nombre_recurso_pedido);
void evaluar_signal(char* nombre_recurso_pedido);

t_log* iniciar_logger_kernel(char* rutaLog, char* nombreProceso, t_log_level level);

#endif