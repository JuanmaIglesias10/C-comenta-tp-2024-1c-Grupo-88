#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "utils.h"
#include "atender_memoria.h"
#include "diccionario_memoria.h"
#include "commons/temporal.h"

// Function declarations
void inicializar_memoria();
void inicializar_config();
void inicializar_variables();
void inicializar_conexiones();
void enviar_tamanio_pagina();
void iniciar_proceso();

t_proceso* crear_proceso(t_list* listaInstrucciones, uint32_t pid, uint32_t tamanio);

t_list* obtener_instrucciones(char* rutaArchivo);

t_codigo_instruccion obtener_codigo_instruccion(char* charParametro);

void obtener_parametros_instruccion(int numParametro, t_instruccion* instrucciones, char* parametros);

void instrucciones_destroy(t_instruccion* instrucciones_a_destruir);

void enviar_instruccion();

void ejecutar_MOV_IN();

t_proceso* buscarProcesoPorPid(uint32_t pid);



#endif /* MEMORIA_H_ */

