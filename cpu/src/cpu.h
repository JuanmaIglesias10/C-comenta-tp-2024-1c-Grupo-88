#ifndef CPU_H_
#define CPU_H_

#include "utils.h"
#include "atender_cpu.h"
#include "diccionario_cpu.h"
#include "instrucciones.h"

void inicializar_cpu();

void inicializar_config();

void inicializar_conexiones();

void inicializar_registros();

void cargar_registros();

void guardar_cde(t_cde* cde);

void ejecutar_proceso(t_cde* cde);

void ejecutar_instruccion(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

void desalojar_cde(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

void devolver_cde_a_kernel(t_cde* cde, t_instruccion* instruccion_a_ejecutar);

#endif /* CPU_H */