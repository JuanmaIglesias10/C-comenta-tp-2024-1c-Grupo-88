#include <../include/main.h>

int main(void) {

    // ------------ MEMORIA SERVER DE CPU -------------
    logger = iniciar_logger("logDeCpu.log", "Servidor de cpu", LOG_LEVEL_INFO);


	int server_fd = iniciar_servidor("8002");
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

/*
t_log* iniciar_logger(void) {

	t_log* nuevo_logger = log_create("log.log", "server" , true, LOG_LEVEL_DEBUG);
	if (nuevo_logger == NULL) {
		printf("no se pudo crear el log");
		exit(3);
	}
	return nuevo_logger;
}
*/

t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level) //Podriamos pasarle el logger por parametro (TODO) 
{
	t_log* nuevo_logger = log_create(rutaLog, nombreProceso , true, level);
	if (nuevo_logger == NULL) {
		printf("no se pudo crear el log");
		exit(3);
	}

	return nuevo_logger;
}
