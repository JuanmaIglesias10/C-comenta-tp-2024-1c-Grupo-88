#include <../include/utils.h>

void chequearErrores(char* tipoError, int status)
{
	if (status == -1) {
    	fprintf(stderr, "%s: %s\n", tipoError, gai_strerror(status));
    	exit(1);
	}
}

t_config* iniciar_config(char* nombreArchivoconfig)
{
	t_config *nuevo_config = config_create(nombreArchivoconfig);
	if (nuevo_config == NULL)
	{
		perror("Hubo un error en las CONFIG");
		exit(EXIT_FAILURE);
	}
	return nuevo_config;
}

t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level)
{
	t_log* nuevo_logger = log_create(rutaLog, nombreProceso, true, level);
	if (nuevo_logger == NULL) {
		printf("no se pudo crear el log");
		exit(3);
	}
	return nuevo_logger;
}

int server_escuchar(t_log *logger, char *puerto)
{
	int server_fd = iniciar_servidor(puerto, logger);
	int cliente_fd = esperar_cliente(server_fd, logger);

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd, logger);
			break;
		case -1:
			log_info(logger, "El cliente se desconecto.");
			break;//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
		log_info(logger, "El cliente se desconecto.");
		cliente_fd = esperar_cliente(server_fd, logger);
	}
	return EXIT_SUCCESS;
}

void conectarse(t_config *config, char *keyIP, char* keyPuerto, char *nombreDelModulo,t_log* logger)
{
	char* ip = config_get_string_value(config, keyIP);
	char* puerto = config_get_string_value(config, keyPuerto);

	int conexion = crear_conexion(ip, puerto);

	char *nombreServer = strchr(keyPuerto, '_');
    if (nombreServer != NULL) {
		nombreServer++; //No imprimo el _
        log_info(logger, "Me conecté a %s", nombreServer);
    } else {
        log_info(logger, "Underscore not found."); // Log si no paso un keyPuerto correcto
    }
	
	char mensaje[100];
	sprintf(mensaje, "Buenas, soy el %s, me conecte", nombreDelModulo);
	enviar_mensaje(mensaje, conexion); 
	liberar_conexion(conexion);
	log_info(logger, "Me desconecte de %s", nombreServer); // TODO
	config_destroy(config);
}