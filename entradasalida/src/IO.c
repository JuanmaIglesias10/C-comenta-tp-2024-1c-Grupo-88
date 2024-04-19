#include <../include/IO.h>

int main(void) {


	// ------------  ENTRADA/SALIDA ------> KERNEL -------------------
	
	t_config* config_IO = iniciar_config("../entradaSalida.config");
	t_log* logger_IO = iniciar_logger("logEntradaSalida.log","cliente",LOG_LEVEL_INFO);

	int fd_kernel = conectarse(config_IO, "IP", "PUERTO_KERNEL", "ENTRADASALIDA", logger_IO);
	log_info(logger_IO,"Conexion con KERNEL exitosa");


	int fd_memoria = conectarse(config_IO, "IP", "PUERTO_MEMORIA", "ENTRADASALIDA", logger_IO);
	log_info(logger_IO,"Conexion con MEMORIA exitosa");


	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_kernel(logger_IO,fd_kernel), NULL);
	pthread_detach(hilo_kernel);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_memoria(logger_IO,fd_memoria), NULL);
	pthread_detach(hilo_memoria);


	liberar_conexion(fd_kernel);
	liberar_conexion(fd_memoria);

	log_destroy(logger_IO);
}

