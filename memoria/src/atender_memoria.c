#include "atender_memoria.h"

void atender_cpu()
{	
	while (1) {
		mensajeCpuMemoria cod_op = recibir_codOp(fd_cpu);
		switch (cod_op) {
			case MOV_IN_SOLICITUD: //Desde ejecutar_mov_in
				ejecutar_MOV_IN();
				break;
			case PEDIDO_INSTRUCCION:
				usleep(config_memoria.retardo_respuesta * 1000);
				enviar_instruccion();
				break;
            case NUMERO_MARCO_SOLICITUD: //Desde calcular_direccion_fisica en mmu.c
				devolver_nro_marco();
				break;
			default:
				log_info(logger_memoria,"Se desconectó CPU");
				return;
		}
	}
}


void atender_kernel()
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

void* atender_IO(void* fd_IO_ptr) {
    int fd_IO = *(int*)fd_IO_ptr;
    free(fd_IO_ptr); // Liberamos la memoria alocada para el descriptor de archivo

    while (1) {
        uint8_t cod_op = recibir_codOp(fd_IO);
        switch (cod_op) {
            case MENSAJE:
                // Manejo del mensaje
                break;
            case PAQUETE:
                // Manejo del paquete
                break;
            default:
                log_info(logger_memoria, "Se desconectó IO");
                close(fd_IO); // Cerramos la conexión
                return NULL;
        }
    }
    return NULL;
}

void* aceptar_conexiones_IO(void* arg) {
    int fd_memoria = *(int*)arg;

    while (1) {
        int fd_IO = esperar_cliente(fd_memoria, logger_memoria, "IO");
        if (fd_IO != -1) {
            pthread_t hilo_IO;
            int* fd_IO_ptr = malloc(sizeof(int));
            *fd_IO_ptr = fd_IO;
            pthread_create(&hilo_IO, NULL, (void*)atender_IO, (void*)fd_IO_ptr);
            pthread_detach(hilo_IO);
        }
    }
}