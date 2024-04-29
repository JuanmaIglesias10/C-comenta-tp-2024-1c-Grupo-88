#include "sockets.h"

int iniciar_servidor(int puerto, t_log* logger)
{

	int socket_servidor;
	char* puerto_recibido = string_itoa(puerto);
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;


	getaddrinfo(NULL, puerto_recibido, &hints, &servinfo);

	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	int val = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	free(puerto_recibido);

	return socket_servidor;
}

int esperar_cliente(int socket_servidor, t_log* logger, char* nombreCliente) {
    log_info(logger, "Servidor listo para recibir a %s", nombreCliente);
    int socket_cliente = accept(socket_servidor, NULL, NULL);
    chequearErrores("accept error", socket_cliente);
    log_info(logger, "Se conectó %s", nombreCliente);

    return socket_cliente;
}


int crear_conexion(char *ip, char* puerto)
{	
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family,
	                    server_info->ai_socktype,
	                    server_info->ai_protocol);

	int val = 1;
	setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(server_info);
	return socket_cliente;
}

int handshake_cliente(int socket_servidor){
    uint32_t handshake = 1;
    uint32_t handshake_result;

    send(socket_servidor, &handshake, sizeof(uint32_t), 0);
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
       send(socket_cliente, &resultOk, sizeof(uint32_t), 0);
	   return 1;
	} else{
       send(socket_cliente, &resultError, sizeof(uint32_t), 0);
	   liberar_conexion(socket_cliente);
	   return -1;
	}
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void chequearErrores(char* tipoError, int status)
{
	if (status == -1) {
    	fprintf(stderr, "%s: %s\n", tipoError, gai_strerror(status));
    	exit(EXIT_FAILURE);
	}
}