#include <../include/sockets.h>

void chequearErrores(char* tipoError, int status)
{
	if (status == -1) {
    	fprintf(stderr, "%s: %s\n", tipoError, gai_strerror(status));
    	exit(1);
	}
}

int iniciar_servidor(char* puerto,*t_log logger)
{

	int socket_servidor; // int fd_escucha
	int status;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, puerto, &hints, &servinfo); // -1 = ERROR, 0 = OK
	chequearErrores("getaddrinfo error", status);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
                        	servinfo->ai_socktype,
                        	servinfo->ai_protocol);
	chequearErrores("socket error", status);

	// Asociamos el socket a un puerto
	status = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen); // -1 = ERROR, 0 = OK
	chequearErrores("bind error", status);

	// Escuchamos las conexiones entrantes
	status = listen(socket_servidor, SOMAXCONN); // -1 = ERROR, 0 = OK
	chequearErrores("listen error", status);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;

	// VER COMO LIBERAR MEMORIA EN CASO DE FALLA free(servinfo);
}

int esperar_cliente(int socket_servidor,t_log* logger){
    
    // Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL); // -1 = ERROR, int = OK
	chequearErrores("accept error", socket_cliente);

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;
	int status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(ip, puerto, &hints, &server_info);
	chequearErrores("getaddrinfo error", status);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
                         		server_info->ai_socktype,
                         		server_info->ai_protocol); // -1 = ERROR, int = OK
	chequearErrores("socket error", socket_cliente);

	// Ahora que tenemos el socket, vamos a conectarlo
	status = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen); // -1 = ERROR
	chequearErrores("connect error", socket_cliente);

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

int server_escuchar(t_log *logger, char *puerto)
{
	int server_fd = iniciar_servidor(puerto);
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

void conectarse(t_config *config, char *ip, char *puerto, char *nombreDelModulo) 
{
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO_MEMORIA");

	char *conexion = crear_conexion(ip, puerto);

	enviar_mensaje("hola, soy el %s", nombreDelModulo, conexion);
	liberar_conexion(conexion);
	config_destroy(config);
}