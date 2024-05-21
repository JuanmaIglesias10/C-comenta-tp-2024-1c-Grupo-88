#ifndef CPU_H_
#define CPU_H_

#include "utils.h"
#include "atender_cpu.h"
#include "diccionario_cpu.h"
#include "instrucciones.h"

void inicializar_cpu();
void inicializar_config();
void inicializar_conexiones();

void ejecutar_proceso(t_cde* cde);
void destruir_instruccion(t_instruccion* instruccion);
#endif /* CPU_H */