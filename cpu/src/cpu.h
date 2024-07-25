#ifndef CPU_H_
#define CPU_H_

#include "utils.h"
#include "atender_cpu.h"
#include "diccionario_cpu.h"
#include "instrucciones.h"
#include "math.h"
#include "mmu.h"

void inicializar_cpu(char* nombre_arch_config);

void inicializar_config(char* nombre_arch_config);

void inicializar_semaforos();

void inicializar_conexiones();

void recibir_tamaño_pagina();

void inicializar_registros();

void cargar_registros();

void* atender_kernel_int();

void guardar_cde(t_cde* cde);

void ejecutar_proceso(t_cde* cde);

void ejecutar_instruccion(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

bool es_bloqueante(t_codigo_instruccion instruccion);

void desalojar_cde(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

void devolver_cde_a_kernel(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

#endif /* CPU_H */