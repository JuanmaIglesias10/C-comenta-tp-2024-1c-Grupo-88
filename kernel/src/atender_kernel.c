#include "atender_kernel.h"

void* atender_memoria()
{
	while (1) {
		uint8_t cod_op = recibir_codOp(fd_memoria);
		switch (cod_op) {
<<<<<<< HEAD
            case MENSAJE:
                //'
                break;
=======
>>>>>>> bc0622d8746bce68cefbdf752b73053b7097895f
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_kernel, "Se desconectó MEMORIA");
                break;
            }
	}
    return NULL;
}


void* atender_IO()
{
	while (1) {
		uint8_t cod_op = recibir_codOp(fd_IO);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_kernel, "Se desconectó IO");
                break;
            }
	}
    return NULL;
}

void* atender_cpu_dis()
{
	while (1) {
		mensajeKernelCpu cod_op = recibir_codOp(fd_cpu_dis);
		switch (cod_op) {
            t_buffer* buffer = buffer_create();
            
            break;
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_kernel, "Se desconectó CPU (DISPATCH)");
                break;
            }
	}
    return NULL;
}

void* atender_cpu_int()
{
	while (1) {
		uint8_t cod_op = recibir_codOp(fd_cpu_int);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_kernel, "Se desconectó CPU (INTERRUPT)");
                break;
            }
	}
    return NULL;
}