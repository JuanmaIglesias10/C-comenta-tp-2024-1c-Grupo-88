#ifndef CAMBIOS_DE_ESTADO_H_
#define CAMBIOS_DE_ESTADO_H_

#include "diccionario_kernel.h"
#include "kernel.h"

// Planificacion a largo plazo
void new_a_ready();
void exec_a_finished();

void ready_a_exec();
void agregar_a_cola_finished(char* razon);
void enviar_de_exec_a_ready();
void enviar_de_exec_a_block();
void enviar_pcb_de_block_a_ready(t_pcb* pcb);
void enviar_pcb_de_block_a_ready_mas(t_pcb* pcb);
void enviar_de_exec_a_ready_mas();

#endif /* CAMBIOS_DE_ESTADO_H_ */