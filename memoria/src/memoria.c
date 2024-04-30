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
	pthread_create(&hilo_memoria_cpu, NULL, (void*)atender_cpu, NULL);
	pthread_detach(hilo_memoria_cpu);


	pthread_t hilo_memoria_kernel;
	pthread_create(&hilo_memoria_kernel, NULL, (void*)atender_kernel, NULL);
	pthread_detach(hilo_memoria_kernel);


	pthread_t hilo_memoria_IO;
	pthread_create(&hilo_memoria_IO, NULL, (void*)atender_IO, NULL);
	pthread_join(hilo_memoria_IO,NULL);
	
	// liberar_conexion(fd_memoria);
}

void iniciar_proceso(){
	//Recibo el buffer 
	t_buffer* bufferMemoria = recibir_buffer(fd_kernel);
	//Recibo el PID y el path del .txt
	uint32_t pid = leer_buffer_uint32(bufferMemoria);
	char* pathKernel = leer_buffer_string(bufferMemoria);
	destruir_buffer(bufferMemoria);
	//Combino el path del .txt con el path del config
	char* rutaCompleta = string_new();
	string_append(&rutaCompleta, config_memoria.path_instrucciones);
	string_append(&rutaCompleta, pathKernel);

	// t_list* listaInstrucciones = parsearArchivo(rutaCompleta, logger_memoria);

}
