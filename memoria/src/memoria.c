#include "memoria.h"

int main(void) {
	
	inicializar_memoria();
	
}

void inicializar_memoria(){
	logger_memoria = iniciar_logger("logMemoria.log", "MEMORIA", LOG_LEVEL_INFO);
	inicializar_config();
	inicializar_conexiones();
}


void inicializar_config(){
	config = config_create("./memoria.config");
	config_memoria.puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	config_memoria.tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
	config_memoria.tam_pagina = config_get_int_value(config, "TAM_PAGINA");
	config_memoria.path_instrucciones = config_get_string_value(config, "PATH_INSTRUCCIONES");
	config_memoria.retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");

}
void inicializar_conexiones() {
	fd_memoria = iniciar_servidor(config_memoria.puerto_escucha, logger_memoria);

	fd_cpu = esperar_cliente(fd_memoria, logger_memoria, "CPU");

	fd_kernel = esperar_cliente(fd_memoria, logger_memoria,"KERNEL"); 

	fd_IO = esperar_cliente(fd_memoria, logger_memoria,"IO"); 


	pthread_t hilo_memoria_cpu;
	pthread_create(&hilo_memoria_cpu, NULL, (void*)atender_cpu(fd_cpu), NULL);
	pthread_detach(hilo_memoria_cpu);

	pthread_t hilo_memoria_kernel;
	pthread_create(&hilo_memoria_kernel, NULL, (void*)atender_kernel(fd_kernel), NULL);
	pthread_detach(hilo_memoria_kernel);

	pthread_t hilo_memoria_IO;
	pthread_create(&hilo_memoria_IO, NULL, (void*)atender_IO( fd_IO), NULL);
	pthread_detach(hilo_memoria_IO);

	liberar_conexion(fd_memoria);


}

