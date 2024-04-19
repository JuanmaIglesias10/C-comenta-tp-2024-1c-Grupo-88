#include "../include/atender_kernel.h"



void* atender_kernel(t_log *logger, int cliente_fd)
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
                log_error(logger, "el cliente KERNEL se desconecto.");
                control_key = 0;
                break;
            default:
                log_warning(logger,"Operacion desconocida. No quieras meter la pata");
                break;
            }
	}
    return 0;
}