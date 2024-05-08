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

// CONSOLA
void iniciar_consola();
void mostrar_opciones_consola();
void atender_consola(char* inpt);
t_codigo_operacion get_codigo_operacion(char* , int );

t_list* ejecutar_script(char* pathScript);
t_list* parsear_archivo_script(char* pathScript);
t_codigo_operacion obtener_codigo_operacion(char* parametro);
void iniciar_proceso(char* path);
t_pcb* crear_PCB();

#endif