#include <../include/memoria.h>

int main(void) {
    // ------------ CPU-KERNEL-ENTRADASALIDA ------> MEMORIA -------------

	t_log* logger_memoria = iniciar_logger("logMemoria.log", "Server", LOG_LEVEL_DEBUG);
	t_config* config_memoria = config_create("../memoria.config");
	char* PUERTO_MEMORIA = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");

	/*						INICIO EL SERVIDOR									*/
	int fd_memoria = iniciar_servidor(PUERTO_MEMORIA, logger_memoria);

	/*						ESPERO AL CLIENTE CPU								*/
	int fd_cpu = esperar_cliente(fd_memoria, logger_memoria, "CPU");

	/*						ESPERO AL CLIENTE KERNEL					     	*/
	int fd_kernel = esperar_cliente(fd_memoria, logger_memoria,"KERNEL"); 

	/*						ESPERO AL CLIENTE ENTRADASALIDA						*/
	int fd_IO = esperar_cliente(fd_memoria, logger_memoria,"IO"); 

	// /*---------------------------------HILOS------------------------------------*/

	pthread_t hilo_memoria_cpu;
	pthread_create(&hilo_memoria_cpu, NULL, (void*)atender_cpu(logger_memoria, fd_cpu), NULL);
	pthread_detach(hilo_memoria_cpu);

	pthread_t hilo_memoria_kernel;
	pthread_create(&hilo_memoria_kernel, NULL, (void*)atender_kernel(logger_memoria, fd_kernel), NULL);
	pthread_join(hilo_memoria_kernel, NULL);

	pthread_t hilo_memoria_IO;
	pthread_create(&hilo_memoria_IO, NULL, (void*)atender_IO(logger_memoria, fd_IO), NULL);
	pthread_detach(hilo_memoria_IO);

	liberar_conexion(fd_memoria);
	// server_escuchar(logger);


}