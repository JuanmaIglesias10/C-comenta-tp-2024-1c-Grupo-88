#include <../include/sockets.h>

int iniciar_servidor(char* puerto,t_log* logger)
{

	int socket_servidor; // int fd_escucha
	int status;

	struct addrinfo hints, *servinfo;

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
	log_info(logger, "Servidor listo para recibir al cliente");
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

int handshake_cliente(int socket_servidor){
    uint32_t handshake = 1;
    uint32_t handshake_result;

    send(socket_servidor, &handshake, sizeof(uint32_t), NULL);
    recv(socket_servidor, &handshake_result, sizeof(uint32_t), MSG_WAITALL);

		if(handshake_result==2){
    	return 1;
		} else {
			liberar_conexion(socket_servidor);
			return -1;
		}
}

int handshake_servidor(int socket_cliente){
   	uint32_t handshake;
    uint32_t resultOk = 2;
    uint32_t resultError = 1;

    recv(socket_cliente, &handshake, sizeof(uint32_t), MSG_WAITALL);
    if(handshake == 1){
       send(socket_cliente, &resultOk, sizeof(uint32_t), NULL);
	   return 1;
	} else{
       send(socket_cliente, &resultError, sizeof(uint32_t), NULL);
	   liberar_conexion(socket_cliente);
	   return -1;
	}
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}