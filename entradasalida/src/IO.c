#include <IO.h>

t_log* logger_IO;
t_config* config_IO;

int fd_kernel;
int fd_memoria;

int main(void) {

	inicializar_IO();

	// ------------  ENTRADA/SALIDA ------> KERNEL -------------------

	fd_kernel = conectarse(config_IO, "IP", "PUERTO_KERNEL", "ENTRADASALIDA", logger_IO);
	log_info(logger_IO,"Conexion con KERNEL exitosa");


	fd_memoria = conectarse(config_IO, "IP", "PUERTO_MEMORIA", "ENTRADASALIDA", logger_IO);
	log_info(logger_IO,"Conexion con MEMORIA exitosa");


	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_kernel(fd_kernel), NULL);
	pthread_detach(hilo_kernel);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_memoria(fd_memoria), NULL);
	pthread_detach(hilo_memoria);


	liberar_conexion(fd_kernel);
	liberar_conexion(fd_memoria);

	log_destroy(logger_IO);
}

void inicializar_IO(){

	logger_IO = iniciar_logger("logEntradaSalida.log","cliente",LOG_LEVEL_INFO);
	config_IO = iniciar_config("./entradaSalida.config");
	
}