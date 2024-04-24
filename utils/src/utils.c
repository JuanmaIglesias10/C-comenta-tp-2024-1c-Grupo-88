#include <utils.h>

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

void* server_escuchar(t_log *logger, int cliente_fd, char* nombreCliente)
{
	int control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			// recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			// lista = recibir_paquete(cliente_fd);
			// log_info(logger, "Me llegaron los siguientes valores:\n");
			// list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente %s se desconecto.", nombreCliente);
			control_key = 0;
			// return EXIT_FAILURE;
			break;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}


int conectarse(t_config *config, char *keyIP, char* keyPuerto, char *nombreDelModulo, t_log* logger)
{
	char* ip = config_get_string_value(config, keyIP);
	char* puerto = config_get_string_value(config, keyPuerto);

	int conexion = crear_conexion(ip, puerto);
	return conexion;

	// char* nombreServer = obtenerNombreServer(keyPuerto);

	// int hs = handshake_cliente(conexion);
	// if(hs<0){
	// 	log_error(logger,"Resultado del handshake incorrecto");
	// 	config_destroy(config);
	// 	return;
	// } else {
	// 	log_info(logger, "Me conecté a %s", nombreServer);
	// }

	// char mensaje[100];
	// sprintf(mensaje, "Buenas, soy el %s, me conecte", nombreDelModulo);
	// enviar_mensaje(mensaje, conexion); 


	// liberar_conexion(conexion);
	// log_info(logger, "Me desconecte de %s", nombreServer);
	config_destroy(config);
}

char* obtenerNombreServer(char* keyPuerto){
	char *nombreServer = strchr(keyPuerto, '_');
	if (nombreServer != NULL) {
		nombreServer++;
		return nombreServer;
	} else {
		return "ERROR_NOMBRE";
	}
}