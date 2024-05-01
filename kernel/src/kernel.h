#ifndef KERNEL_H_
#define KERNEL_H_

#include <diccionario_kernel.h>
#include "consola.h"
#include <atender_kernel.h>
#include <utils.h>


// CONEXIONES
void inicializar_kernel();
void inicializarListasColas();
void inicializar_conexiones();
void iniciar_config_kernel();

// CONSOLA
void inciar_consola();
void mostrar_opciones_consola();
void atender_consola(char* inpt);
t_codigo_operacion get_codigo_operacion(char* , int );

void iniciarProceso(char* path);
t_pcb* crear_PCB(char* path);

#endif