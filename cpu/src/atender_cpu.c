#include "atender_cpu.h"

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
                return NULL;
            }
	}
}


