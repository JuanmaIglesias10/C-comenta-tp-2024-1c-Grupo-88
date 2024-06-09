#ifndef CPU_H_
#define CPU_H_

#include "utils.h"
#include "atender_cpu.h"
#include "diccionario_cpu.h"
#include "instrucciones.h"

void inicializar_cpu();

void inicializar_config();

void inicializar_semaforos();

void inicializar_conexiones();

void inicializar_registros();

void cargar_registros();

void* atender_kernel_int();

void guardar_cde(t_cde* cde);

void ejecutar_proceso(t_cde* cde);

void ejecutar_instruccion(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

bool es_bloqueante(t_codigo_instruccion instruccion);

void desalojar_cde(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

void devolver_cde_a_kernel(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

int obtener_nro_pagina(int direccionLogica);

int obtener_desplazamiento_pagina(int direccionLogica);

uint32_t calcular_direccion_fisica(int direccion_logica, t_cde* cde);

#endif /* CPU_H */