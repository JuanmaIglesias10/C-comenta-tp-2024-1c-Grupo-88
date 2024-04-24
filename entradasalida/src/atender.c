#include <atender.h>

void* atender_memoria( int cliente_fd)
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
                log_error(logger_IO, "el cliente MEMORIA se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_IO,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}

void* atender_kernel( int cliente_fd)
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
                log_error(logger_IO, "el cliente KERNEL se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger_IO,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}