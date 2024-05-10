#include "atender_cpu.h"

void* atender_memoria()
{
	while (1) {
		uint8_t cod_op = recibir_codOp(fd_memoria);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_cpu, "Se desconectó MEMORIA");
                break;
            }
	}
    return 0;
}

void* atender_kernel_dis()
{
    //t_buffer* buffer = recibir_buffer(fd_kernel_dis);
	while (1) {
		mensajeKernelCpu cod_op = recibir_codOp(fd_kernel_dis);
		switch (cod_op) {
            case MENSAJE:
                //t_cde* cde = malloc(sizeof(t_cde));
                //cde->pid = leer_buffer_uint32(buffer);
                //cde->program_counter = leer_buffer_uint32(buffer);
                //cde->registros = buffer_read_registros(buffer);	
                //destruir_buffer(buffer);
                //ejecutar_proceso(cde);
                break;
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_cpu, "Se desconectó KERNEL (DISPATCH)");
                break;
            }
	}
    return 0;
}

void* atender_kernel_int()
{
	while (1) {
		uint8_t cod_op = recibir_codOp(fd_kernel_int);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_cpu, "Se desconectó KERNEL (INTERRUPT)");
                break;
            }
	}
    return 0;
}

