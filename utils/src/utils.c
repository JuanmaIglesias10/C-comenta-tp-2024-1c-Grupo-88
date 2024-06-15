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



void destruir_cde(t_cde* cde){
    free(cde->registros);
    free(cde);
}

void destruir_instruccion(t_instruccion* instruccion) {
    if (instruccion == NULL) return;

    if (instruccion->par1 != NULL) {
        free(instruccion->par1);
        instruccion->par1 = NULL;
    }
    if (instruccion->par2 != NULL) {
        free(instruccion->par2);
        instruccion->par2 = NULL;
    }
    if (instruccion->par3 != NULL) {
        free(instruccion->par3);
        instruccion->par3 = NULL;
    }
    if (instruccion->par4 != NULL) {
        free(instruccion->par4);
        instruccion->par4 = NULL;
    }
    if (instruccion->par5 != NULL) {
        free(instruccion->par5);
        instruccion->par5 = NULL;
    }
    free(instruccion);
}
