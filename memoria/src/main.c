#include <../include/main.h>

int main(void) {
    // ------------ CPU-KERNEL-ENTRADASALIDA ------> MEMORIA -------------

	t_log* logger = iniciar_logger("logMemoria.log", "Server", LOG_LEVEL_DEBUG);


	/*						INICIO EL SERVIDOR									*/
	int fd_memoria = iniciar_servidor("8003", logger);

	/*						ESPERO AL CLIENTE CPU								*/
	int fd_cpu = esperar_cliente(fd_memoria, logger, "CPU");

	/*						ESPERO AL CLIENTE KERNEL					     	*/
	int fd_kernel = esperar_cliente(fd_memoria, logger,"KERNEL"); 

	/*						ESPERO AL CLIENTE ENTRADASALIDA						*/
	int fd_entradasalida = esperar_cliente(fd_memoria, logger,"ENTRADASALIDA"); 


	pthread_t hilo_memoria_cpu;
	pthread_t hilo_memoria_kernel;
	pthread_t hilo_memoria_entradasalida;

	pthread_create(&hilo_memoria_cpu, NULL, (void*)server_escuchar(logger,fd_cpu,"CPU"), NULL);
	pthread_detach(hilo_memoria_cpu);

	pthread_create(&hilo_memoria_kernel, NULL, (void*)server_escuchar(logger,fd_kernel,"KERNEL"), NULL);
	pthread_detach(hilo_memoria_kernel);

	pthread_create(&hilo_memoria_entradasalida, NULL, (void*)server_escuchar(logger,fd_entradasalida,"ENTRADASALIDA"), NULL);
	pthread_join(hilo_memoria_entradasalida, NULL);

	liberar_conexion(fd_memoria);
	// server_escuchar(logger);




}