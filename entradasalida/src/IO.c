#include "IO.h"

int main(int argc, char* argv[]) {
	char* nombre_interfaz = argv[1];
	char* path_archivo_config = argv[2];
	inicializar_IO(nombre_interfaz , path_archivo_config);
}

void inicializar_IO(char* nombre_interfaz, char* path_archivo_config){

	logger_IO = iniciar_logger("logEntradaSalida.log", nombre_interfaz ,LOG_LEVEL_INFO);
	config = config_create(path_archivo_config);
	char* tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
	if (strcmp(tipo_interfaz,"GENERICA") == 0){
		inicializar_config_IO_GENERICA();
		inicializar_IO_generica();
		enviar_info_kernel(nombre_interfaz,tipo_interfaz);
		exec_IO_Generica();
	} else if (strcmp(tipo_interfaz,"STDIN") == 0){
		inicializar_config_IO_STDIN();
		inicializar_IO_STDIN();
		enviar_info_kernel(nombre_interfaz,tipo_interfaz);
	} else if (strcmp(tipo_interfaz,"STDOUT") == 0){
		inicializar_config_IO_STDOUT();
		inicializar_IO_STDOUT();
		enviar_info_kernel(nombre_interfaz,tipo_interfaz);
	} else {
		inicializar_config_IO_DIALFS();
		inicializar_IO_DIALFS();
		enviar_info_kernel(nombre_interfaz,tipo_interfaz);
	}
	inicializar_conexiones();
}

void inicializar_config_IO_GENERICA(){
	config_IO_GENERICA.tipo_interfaz= config_get_string_value(config,"TIPO_INTERFAZ");
	config_IO_GENERICA.tiempo_unidad_trabajo = config_get_int_value(config,"TIEMPO_UNIDAD_TRABAJO"); 
	config_IO_GENERICA.ip_kernel = config_get_string_value(config,"IP_KERNEL"); 
	config_IO_GENERICA.puerto_kernel= config_get_int_value(config,"PUERTO_KERNEL");
}

void inicializar_config_IO_STDIN(){
	config_IO_STDIN.tipo_interfaz= config_get_string_value(config,"TIPO_INTERFAZ");
	config_IO_STDIN.ip_kernel = config_get_string_value(config,"IP_KERNEL");
	config_IO_STDIN.puerto_kernel= config_get_int_value(config,"PUERTO_KERNEL");
	config_IO_STDIN.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_IO_STDIN.puerto_memoria=config_get_int_value(config,"PUERTO_MEMORIA");
}

void inicializar_config_IO_STDOUT(){
	config_IO_STDOUT.tipo_interfaz= config_get_string_value(config,"TIPO_INTERFAZ");
	config_IO_STDOUT.ip_kernel = config_get_string_value(config,"IP_KERNEL");
	config_IO_STDOUT.puerto_kernel= config_get_int_value(config,"PUERTO_KERNEL");
	config_IO_STDOUT.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_IO_STDOUT.puerto_memoria=config_get_int_value(config,"PUERTO_MEMORIA");
}

void inicializar_config_IO_DIALFS(){
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
}

void inicializar_IO_generica(char* nombre_interfaz, char* tipo) {
	fd_kernel = conectarse(config_IO_GENERICA.ip_kernel,config_IO_GENERICA.puerto_kernel, "KERNEL", logger_IO);
}

void inicializar_IO_STDIN(){
	fd_memoria = conectarse(config_IO_STDIN.ip_memoria,config_IO_STDIN.puerto_memoria, "MEMORIA", logger_IO);
	fd_kernel = conectarse(config_IO_STDIN.ip_kernel,config_IO_STDIN.puerto_kernel, "KERNEL", logger_IO);
	if(fd_kernel == -1) exit(1);
	while(1);
}

void inicializar_IO_STDOUT() {
	fd_memoria = conectarse(config_IO_STDOUT.ip_memoria,config_IO_STDOUT.puerto_memoria, "MEMORIA", logger_IO);
	fd_kernel = conectarse(config_IO_STDOUT.ip_kernel,config_IO_STDOUT.puerto_kernel, "KERNEL", logger_IO);
}

void inicializar_IO_DIALFS() {
	fd_memoria = conectarse(config_IO_DIALFS.ip_memoria,config_IO_DIALFS.puerto_memoria, "MEMORIA", logger_IO);
	fd_kernel = conectarse(config_IO_DIALFS.ip_kernel,config_IO_DIALFS.puerto_kernel, "KERNEL", logger_IO);
}


void inicializar_conexiones(){
	
	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_memoria, NULL);
	pthread_detach(hilo_memoria);

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_kernel, NULL);
	pthread_join(hilo_kernel, NULL);


	// liberar_conexion(fd_kernel);
	// liberar_conexion(fd_memoria);

	// log_destroy(logger_IO);
}

void enviar_info_kernel(char* nombre_interfaz, char* tipo){
	
	t_buffer* buffer = crear_buffer();
	agregar_buffer_string(buffer,nombre_interfaz);
	agregar_buffer_string(buffer,tipo);
	enviar_buffer(buffer,fd_kernel);
	destruir_buffer(buffer);
}


void exec_IO_Generica() {

	// uint8_t codigo = recibir_codOp(fd_kernel);
	
	// if (codigo == IO_GEN_SLEEP) { 
	t_buffer* buffer_recibido = recibir_buffer(fd_kernel);
	uint8_t unidadesDeTiempo = leer_buffer_uint8(buffer_recibido);
	usleep(unidadesDeTiempo * 1000);

	log_info(logger_IO, "LLEGUE ACA");
		// 	CUANDO LA IO HACE SLEEP EL PROCESO DEBERIA BLOQUEARSE, NO OLVIDARSE DE CODEAR ESO!!!
	enviar_codOp(fd_kernel,SLEEP_OK);
	// }
}
