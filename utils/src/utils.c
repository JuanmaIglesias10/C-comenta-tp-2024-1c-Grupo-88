#include "utils.h"


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
		exit(1);
	}
	return nuevo_logger;
}

int conectarse(char* IP, int puerto, char* nombreConexion, t_log* logger){
	char* puerto_conectado = string_itoa(puerto);
	int conexion = crear_conexion(IP, puerto_conectado);
	log_info(logger, "Conexion exitosa con %s" , nombreConexion);
	return conexion;
} 


// Enviar y Recibir codigo operacion. (PARA ENUMS)
void enviar_codOp(int fd_servidor , uint8_t codOp){
	send(fd_servidor, &codOp , sizeof(uint8_t) , 0);
}
//
//t_registros* buffer_read_registros(t_buffer* buffer){
//	t_registros* registros = malloc(sizeof(t_registros));
//
//	registros->AX = leer_buffer_uint8(buffer);
//	registros->BX = leer_buffer_uint8(buffer);
//	registros->CX = leer_buffer_uint8(buffer);
//	registros->DX = leer_buffer_uint8(buffer);
// 	registros->EAX = leer_buffer_uint32(buffer);
// 	registros->EBX = leer_buffer_uint32(buffer);
//	registros->ECX = leer_buffer_uint32(buffer);
//	registros->EDX = leer_buffer_uint32(buffer);
//	registros->SI = leer_buffer_uint32(buffer);
//	registros->DI = leer_buffer_uint32(buffer);
//
//	return registros;
//}
