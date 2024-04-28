#include "IO.h"

int main(void) {

	inicializar_IO();
}

void inicializar_IO(){

	logger_IO = iniciar_logger("logEntradaSalida.log","IO",LOG_LEVEL_INFO);
	inicializar_config_IO();
	inicializar_conexiones();
}

void inicializar_config_IO(){
	config = config_create("./entradaSalida.config");
	config_IO.tipo_interfaz= config_get_string_value(config,"TIPO_INTERFAZ");
	config_IO.tiempo_unidad_trabajo = config_get_int_value(config,"TIEMPO_UNIDAD_TRABAJO"); 
	config_IO.ip_kernel = config_get_string_value(config,"IP_KERNEL"); 
	config_IO.puerto_kernel= config_get_int_value(config,"PUERTO_KERNEL");
	config_IO.ip_memoria = config_get_string_value(config,"IP_MEMORIA"); 
	config_IO.puerto_memoria= config_get_int_value(config,"PUERTO_MEMORIA");	
	config_IO.path_base_dialfs = config_get_string_value(config,"PATH_BASE_DIALFS"); 
	config_IO.block_size = config_get_int_value(config,"BLOCK_SIZE"); 
	config_IO.block_count = config_get_int_value(config,"BLOCK_COUNT"); 
	config_IO.retraso_compactacion = config_get_int_value(config,"RETRASO_COMPACTACION"); 
}


void inicializar_conexiones(){
	fd_kernel = conectarse(config_IO.ip_kernel,config_IO.puerto_kernel, "KERNEL", logger_IO);
	fd_memoria = conectarse(config_IO.ip_memoria,config_IO.puerto_memoria, "MEMORIA", logger_IO);

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