#include "atender_memoria.h"

void* atender_cpu()
{	
	while (1) {
		mensajeCpuMemoria cod_op = recibir_codOp(fd_cpu);
		
		switch (cod_op) {
			case PEDIDO_INSTRUCCION:
				usleep(config_memoria.retardo_respuesta * 1000);
				enviar_instruccion();
				break;
			default:
				log_info(logger_memoria,"Se desconectó CPU");
				return;
		}
	}
}


void* atender_kernel()
{
	while(1){
		mensajeKernelMem codigoDeKernel = recibir_codOp(fd_kernel);

		switch(codigoDeKernel){
			case INICIAR_PROCESO_SOLICITUD:
				iniciar_proceso();
				break;
			case FINALIZAR_PROCESO_SOLICITUD:
				// finalizarProceso();
				break;
			case PAGE_FAULT_SOLICITUD:
				// atender_page_fault();
				break;
			default:
				log_info(logger_memoria,"Se desconectó Kernel");
				return;
		}
	}
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
			log_info(logger_memoria,"Se desconectó IO");
			return;
		}
	}
}