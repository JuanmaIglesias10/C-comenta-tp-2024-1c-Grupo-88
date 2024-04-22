#include "../include/atender.h"

void* atender_memoria(int cliente_fd)
{
	int control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            case -1:
                log_error(logger_cpu, "el cliente MEMORIA se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_cpu,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}

void* atender_kernel_dis(int cliente_fd)
{
	int control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            case -1:
                log_error(logger_cpu, "el cliente KERNEL (DISPATCH) se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_cpu,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}

void* atender_kernel_int(int cliente_fd)
{
	int control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            case -1:
                log_error(logger_cpu, "el cliente KERNEL (INTERRUPT) se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_cpu,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}

