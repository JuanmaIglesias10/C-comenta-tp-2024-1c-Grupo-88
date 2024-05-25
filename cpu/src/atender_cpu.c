#include "atender_cpu.h"

void* atender_memoria()
{
	while (1) {
		// uint8_t cod_op = recibir_codOp(fd_memoria);
		// switch (cod_op) {
        //     case MENSAJE:
        //         //
        //         break;
        //     case PAQUETE:
        //         //
        //         break;
        //     default:
        //         log_info(logger_cpu, "Se desconectó MEMORIA");
        //         break;
        //     }
	}
    return 0;
}

void* atender_kernel_dis()
{
	while (1) {
		mensajeKernelCpu cod_op = recibir_codOp(fd_kernel_dis);
		switch (cod_op) {
            case EJECUTAR_PROCESO:
                t_buffer* buffer = recibir_buffer(fd_kernel_dis);
                t_cde* cde = leer_buffer_cde(buffer);
                destruir_buffer(buffer);
                
                pthread_mutex_lock(&mutex_cde_ejecutando);
                pid_de_cde_ejecutando = cde->pid;
                pthread_mutex_unlock(&mutex_cde_ejecutando);
                
                ejecutar_proceso(cde);
                break;
            case ALGORITMO_PLANIFICACION:
                t_buffer* buffer_pid = recibir_buffer(fd_kernel_dis);
                algoritmo_planificacion = leer_buffer_uint8(buffer_pid);
                destruir_buffer(buffer_pid);
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
		mensajeKernelCpu cod_op = recibir_codOp(fd_kernel_int);
		switch (cod_op) {
            case DESALOJO:
                t_buffer* buffer = recibir_buffer(fd_kernel_int); // recibe pid o lo que necesite
                uint32_t pid_recibido = leer_buffer_uint32(buffer);
                // se desaloja proceso en ejecucion
                if(algoritmo_planificacion == 1 && pid_de_cde_ejecutando != pid_recibido){ // significa que el algoritmo es RR
                    break;
                }
                // else if(algoritmo_planificacion == 1 && pid_de_cde_ejecutando == pid_recibido){
                //      if(es_bloqueante(instruccion_actualizada)){ //Esto hay que hacerlo para la interfaz generica
                //         break;
                //     }
                // }
                pthread_mutex_lock(&mutex_realizar_desalojo);
                realizar_desalojo = 1;
                pthread_mutex_unlock(&mutex_realizar_desalojo);
                break;
            default:
                log_info(logger_cpu, "Se desconectó KERNEL (INTERRUPT)");
                break;
            }
	}
    return 0;
}

