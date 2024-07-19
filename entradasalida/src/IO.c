#include "IO.h"

int main(int argc, char* argv[]) {
	char* nombre_interfaz = argv[1];
	char* path_archivo_config = argv[2];
	seleccionar_tipo_interfaz(nombre_interfaz , path_archivo_config);
}

void seleccionar_tipo_interfaz(char* nombre_interfaz, char* path_archivo_config){

	logger_IO = iniciar_logger("logEntradaSalida.log", nombre_interfaz ,LOG_LEVEL_DEBUG);
	config = config_create(path_archivo_config);
	char* tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
	if (strcmp(tipo_interfaz,"GENERICA") == 0){
		inicializar_IO_GENERICA(nombre_interfaz);
	} else if (strcmp(tipo_interfaz,"STDIN") == 0){
		inicializar_IO_STDIN(nombre_interfaz);
	} else if (strcmp(tipo_interfaz,"STDOUT") == 0){
		inicializar_IO_STDOUT(nombre_interfaz);
	} else { // DIALFS
		inicializar_IO_DIALFS(nombre_interfaz);
	}

}

void inicializar_IO_GENERICA(char* nombre_interfaz) {
	config_IO_GENERICA.tipo_interfaz= config_get_string_value(config,"TIPO_INTERFAZ");
	config_IO_GENERICA.tiempo_unidad_trabajo = config_get_int_value(config,"TIEMPO_UNIDAD_TRABAJO"); 
	config_IO_GENERICA.ip_kernel = config_get_string_value(config,"IP_KERNEL"); 
	config_IO_GENERICA.puerto_kernel= config_get_int_value(config,"PUERTO_KERNEL");

	fd_kernel = conectarse(config_IO_GENERICA.ip_kernel,config_IO_GENERICA.puerto_kernel, "KERNEL", logger_IO);

	enviar_info_kernel(nombre_interfaz,config_IO_GENERICA.tipo_interfaz);

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_kernel_IO_GENERICA, NULL);
	pthread_join(hilo_kernel, NULL);
}

void inicializar_IO_STDIN(char* nombre_interfaz){
	config_IO_STDIN.tipo_interfaz= config_get_string_value(config,"TIPO_INTERFAZ");
	config_IO_STDIN.ip_kernel = config_get_string_value(config,"IP_KERNEL");
	config_IO_STDIN.puerto_kernel= config_get_int_value(config,"PUERTO_KERNEL");
	config_IO_STDIN.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_IO_STDIN.puerto_memoria=config_get_int_value(config,"PUERTO_MEMORIA");

	fd_memoria = conectarse(config_IO_STDIN.ip_memoria,config_IO_STDIN.puerto_memoria, "MEMORIA", logger_IO);
	fd_kernel = conectarse(config_IO_STDIN.ip_kernel,config_IO_STDIN.puerto_kernel, "KERNEL", logger_IO);

	enviar_info_kernel(nombre_interfaz,config_IO_STDIN.tipo_interfaz);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_memoria_IO_STDIN, NULL);
	pthread_detach(hilo_memoria);

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_kernel_IO_STDIN, NULL);
	pthread_join(hilo_kernel, NULL);
}

void inicializar_IO_STDOUT(char* nombre_interfaz){
	config_IO_STDOUT.tipo_interfaz= config_get_string_value(config,"TIPO_INTERFAZ");
	config_IO_STDOUT.ip_kernel = config_get_string_value(config,"IP_KERNEL");
	config_IO_STDOUT.puerto_kernel= config_get_int_value(config,"PUERTO_KERNEL");
	config_IO_STDOUT.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_IO_STDOUT.puerto_memoria=config_get_int_value(config,"PUERTO_MEMORIA");

	fd_memoria = conectarse(config_IO_STDOUT.ip_memoria,config_IO_STDOUT.puerto_memoria, "MEMORIA", logger_IO);
	fd_kernel = conectarse(config_IO_STDOUT.ip_kernel,config_IO_STDOUT.puerto_kernel, "KERNEL", logger_IO);

	enviar_info_kernel(nombre_interfaz,config_IO_STDOUT.tipo_interfaz);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_memoria_IO_STDOUT, NULL);
	pthread_detach(hilo_memoria);

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_kernel_IO_STDOUT, NULL);
	pthread_join(hilo_kernel, NULL);
}

void inicializar_IO_DIALFS(char* nombre_interfaz){
	config_IO_DIALFS.tipo_interfaz= config_get_string_value(config,"TIPO_INTERFAZ");
	config_IO_DIALFS.tiempo_unidad_trabajo = config_get_int_value(config,"TIEMPO_UNIDAD_TRABAJO"); 
	config_IO_DIALFS.ip_kernel = config_get_string_value(config,"IP_KERNEL"); 
	config_IO_DIALFS.puerto_kernel= config_get_int_value(config,"PUERTO_KERNEL");
	config_IO_DIALFS.ip_memoria = config_get_string_value(config,"IP_MEMORIA"); 
	config_IO_DIALFS.puerto_memoria= config_get_int_value(config,"PUERTO_MEMORIA");	
	config_IO_DIALFS.path_base_dialfs = config_get_string_value(config,"PATH_BASE_DIALFS"); 
	config_IO_DIALFS.block_size = config_get_int_value(config,"BLOCK_SIZE"); 
	config_IO_DIALFS.block_count = config_get_int_value(config,"BLOCK_COUNT"); 
	config_IO_DIALFS.retraso_compactacion = config_get_int_value(config,"RETRASO_COMPACTACION");

	// testeo_FS();

	fd_memoria = conectarse(config_IO_DIALFS.ip_memoria,config_IO_DIALFS.puerto_memoria, "MEMORIA", logger_IO);
	fd_kernel = conectarse(config_IO_DIALFS.ip_kernel,config_IO_DIALFS.puerto_kernel, "KERNEL", logger_IO);

	enviar_info_kernel(nombre_interfaz,config_IO_DIALFS.tipo_interfaz);

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_kernel_IO_DIALFS, NULL);
	pthread_join(hilo_kernel, NULL);
}

// si esto no se hace antes de crear los hilos despues no se pueden conectar y desconectar las interfaces
void enviar_info_kernel(char* nombre_interfaz, char* tipo){
	
	t_buffer* buffer = crear_buffer();
	agregar_buffer_string(buffer,nombre_interfaz);
	agregar_buffer_string(buffer,tipo);
	enviar_buffer(buffer,fd_kernel);
	destruir_buffer(buffer);
} 