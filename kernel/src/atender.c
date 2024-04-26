#include <atender.h>

void* atender_memoria(int cliente_fd)
{
	int control_key = 1;
	while (control_key) {
		int cod_op = recibir_codOp(cliente_fd);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            case -1:
                log_error(logger_kernel, "el cliente MEMORIA se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_kernel,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}


    void* atender_IO(int cliente_fd)
{
	int control_key = 1;
	while (control_key) {
		int cod_op = recibir_codOp(cliente_fd);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            case -1:
                log_error(logger_kernel, "el cliente IO se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_kernel,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}


void* atender_cpu_int(int cliente_fd)
{
	int control_key = 1;
	while (control_key) {
		int cod_op = recibir_codOp(cliente_fd);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            case -1:
                log_error(logger_kernel, "el cliente CPU (INTERRUPT) se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_kernel,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}

void* atender_cpu_dis(int cliente_fd)
{
	int control_key = 1;
	while (control_key) {
		int cod_op = recibir_codOp(cliente_fd);
		switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            case -1:
                log_error(logger_kernel, "el cliente CPU (DISPATCH) se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_kernel,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}