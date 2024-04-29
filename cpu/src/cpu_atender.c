#include "atender.h"

void* atender_memoria()
{
	while (1) {
		int cod_op = recibir_codOp(fd_memoria);
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
	while (1) {
		int cod_op = recibir_codOp(fd_kernel_dis);
		switch (cod_op) {
            case MENSAJE:
                //
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
		int cod_op = recibir_codOp(fd_kernel_int);
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

