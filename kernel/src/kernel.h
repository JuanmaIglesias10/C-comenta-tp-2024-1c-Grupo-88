#ifndef KERNEL_H_
#define KERNEL_H_

#include <diccionario_kernel.h>
#include <atender.h>
#include <utils.h>


// CONEXIONES
void inicializar_kernel();
void inicializar_conexiones();
void iniciar_config_kernel();

// CONSOLA
void inciar_consola();
void mostrar_opciones_consola();
void atender_consola(char* inpt);
t_codigo_operacion get_codigo_operacion(char* , int );

t_pcb* creando_PCB(char*, int );

#endif