#include "atender_kernel.h"

void* atender_memoria()
{   
	while (1) {
		// uint8_t cod_op = recibir_codOp(fd_memoria);
		// switch (cod_op) {
        //     case MENSAJE:
        //         //'
        //         break;
        //     case PAQUETE:
        //         log_info(logger_kernel,"xd");
        //         break;
        //     default:
        //         log_info(logger_kernel, "Se desconectó MEMORIA");
        //         break;
        //     }
		sleep(5);
	}
}


void* atender_IO(void* arg) {
    int fd_IO = *(int*)arg;
    free(arg);

    while (1) {
        uint8_t cod_op = recibir_codOp(fd_IO);
        switch (cod_op) {
            case MENSAJE:
                // Manejar el mensaje
                break;
            case PAQUETE:
                // Manejar el paquete
                break;
            default:
                log_info(logger_kernel, "Se desconectó IO");
                close(fd_IO);
                return NULL;
        }
    }
}


//Este hace falta para recibir el cde
// void* atender_cpu_dis()
// {
// 	while (1) {
// 		mensajeKernelCpu cod_op = recibir_codOp(fd_cpu_dis);
// 		switch (cod_op) {
//             case MENSAJE:
//             // t_buffer* buffer = buffer_create();
            
//             break;
//             case PAQUETE:
//                 //
//                 break;
//             default:
//                 log_info(logger_kernel, "Se desconectó CPU (DISPATCH)");
//                 break;
//             }
// 	}
//     return NULL;
// }

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
                return;
            }
	}
}

void* aceptar_conexiones_IO(void* arg) {
    int fd_kernel = *(int*)arg;

    while (1) {
        int fd_IO = esperar_cliente(fd_kernel, logger_kernel, "IO");
        if (fd_IO != -1) {
            pthread_t hilo_IO;
            int* fd_IO_ptr = malloc(sizeof(int));
            *fd_IO_ptr = fd_IO;
            pthread_create(&hilo_IO, NULL, (void*)atender_IO, (void*)fd_IO_ptr);
            pthread_detach(hilo_IO);
        }
    }
}