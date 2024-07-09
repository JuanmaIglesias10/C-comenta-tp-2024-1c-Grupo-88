#include "atender_memoria.h"

void atender_cpu()
{	
	while (1) {
		mensajeCpuMemoria cod_op = recibir_codOp(fd_cpu);
		switch (cod_op) {
			case MOV_IN_SOLICITUD: //Desde ejecutar_mov_in
				usleep(config_memoria.retardo_respuesta * 1000);
				ejecutar_MOV_IN();
				break;
			case MOV_OUT_SOLICITUD: //Desde ejecutar_mov_in
				usleep(config_memoria.retardo_respuesta * 1000);
				ejecutar_MOV_OUT();
				break;
			case PEDIDO_INSTRUCCION:
				usleep(config_memoria.retardo_respuesta * 1000);
				enviar_instruccion();
				break;
            case NUMERO_MARCO_SOLICITUD: //Desde calcular_direccion_fisica en mmu.c
				usleep(config_memoria.retardo_respuesta * 1000);
				devolver_nro_marco();
				break;
			case RESIZE_SOLICITUD:
				usleep(config_memoria.retardo_respuesta * 1000);
				resize();
				break;
			case COPY_STRING_SOLICITUD:
				usleep(config_memoria.retardo_respuesta * 1000);
				ejecutar_copy_string();
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
        mensajeIOMemoria cod_op = recibir_codOp(fd_IO);
        switch (cod_op) {
            case IO_M_STDIN_READ_SOLICITUD:
				escribir_stdin_read(fd_IO);
                break;
            case IO_M_STDOUT_WRITE_SOLICITUD:
				leer_stdout_write(fd_IO);
                break;

            default:
                log_info(logger_memoria, "Se desconectó IO");
                close(fd_IO); // Cerramos la conexión
                return NULL;
        }
    }
    return NULL;
} // TODO: va a tener que recibir mensajes de STDIN, STDOUT y DIALFS

void* aceptar_conexiones_IO(void* arg) {
    int fd_memoria = *(int*)arg;

    while (1) {
        int fd_IO = esperar_cliente(fd_memoria, logger_memoria, "una interfaz de IO");
        if (fd_IO != -1) {
            pthread_t hilo_IO;
            int* fd_IO_ptr = malloc(sizeof(int));
            *fd_IO_ptr = fd_IO;
            pthread_create(&hilo_IO, NULL, (void*)atender_IO, (void*)fd_IO_ptr);
            pthread_detach(hilo_IO);
        }
    }
}