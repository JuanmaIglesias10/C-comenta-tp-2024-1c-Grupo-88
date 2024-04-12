#include <../include/main.h>

int main(void) {


	// ----------- CPU SERVER DE KERNEL ------------------------
  	logger = log_create("logServer.log", "ServidorDeKernel",true,LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor("4445");
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


	// ------------  CPU CLIENTE DE MEMORIA -------------------
	int conexion;
	char *ip;
	char *puerto;
	
	t_config *config =iniciar_config();
	t_log* loggerClient = iniciar_logger("logCliente.log","cliente",LOG_LEVEL_INFO);

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO_MEMORIA");

	conexion = crear_conexion(ip, puerto);

	enviar_mensaje("hola, soy el cpu", conexion);
	liberar_conexion(conexion);
	config_destroy(config); 
	
}


t_config *iniciar_config(void)
{
	t_config *nuevo_config = config_create("../cpu.config");
	if (nuevo_config == NULL)
	{
		perror("Hubo un error en las CONFIG");
		exit(EXIT_FAILURE);
	}
	return nuevo_config;
}



t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level) //Podriamos pasarle el logger por parametro (TODO) 
{
	t_log* nuevo_logger = log_create(rutaLog, nombreProceso , true, level);
	if (nuevo_logger == NULL) {
		printf("no se pudo crear el log");
		exit(3);
	}
	return nuevo_logger;

	log_destroy(nuevo_logger);
}
