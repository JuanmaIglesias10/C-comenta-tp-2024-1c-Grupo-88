#include "atender.h"

void* atender_cpu( int cliente_fd)
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
			log_error(logger_memoria, "el cliente CPU se desconecto.");
			control_key = 0;
			// return EXIT_FAILURE;
			break;
		default:
			log_warning(logger_memoria,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return 0;
}


void* atender_kernel(int cliente_fd)
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
			log_error(logger_memoria, "el cliente KERNEL se desconecto.");
			control_key = 0;
			// return EXIT_FAILURE;
			break;
		default:
			log_warning(logger_memoria,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return 0;
}

void* atender_IO( int cliente_fd)
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
			log_error(logger_memoria, "el cliente IO se desconecto.");
			control_key = 0;
			// return EXIT_FAILURE;
			break;
		default:
			log_warning(logger_memoria,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return 0;
}