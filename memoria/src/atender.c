#include "atender.h"

void* atender_cpu()
{	
	while (1) {
		int cod_op = recibir_codOp(fd_cpu);
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
			//
			break;
		default:
			log_info(logger_memoria,"Se desconectó CPU");
			break;
		}
	}
	return NULL;
}


void* atender_kernel()
{
	while (1) {
		int cod_op = recibir_codOp(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
			//
			break;
		default:
			log_info(logger_memoria,"Se desconectó KERNEL");
			break;
		}
	}
	return NULL;
}

void* atender_IO()
{
	while (1) {
		int cod_op = recibir_codOp(fd_IO);
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
			//
			break;
		default:
			log_info(logger_memoria,"Se desconectó KERNEL");
			break;
		}
	}
	return NULL;
}