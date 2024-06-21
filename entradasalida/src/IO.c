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

void inicializar_IO_generica() {
	fd_kernel = conectarse(config_IO_GENERICA.ip_kernel,config_IO_GENERICA.puerto_kernel, "KERNEL", logger_IO);
}

void inicializar_IO_STDIN(){
	fd_memoria = conectarse(config_IO_STDIN.ip_memoria,config_IO_STDIN.puerto_memoria, "MEMORIA", logger_IO);
	fd_kernel = conectarse(config_IO_STDIN.ip_kernel,config_IO_STDIN.puerto_kernel, "KERNEL", logger_IO);
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

void exec_IO_STDIN_READ(){

	t_buffer* buffer_recibido = recibir_buffer(fd_kernel);
    uint32_t dir_fisica= leer_buffer_uint32(buffer_recibido);
	uint32_t tam = leer_buffer_uint32(buffer_recibido);

	destruir_buffer(buffer_recibido);
	int contador = 0;
	while(contador != tam){
		char* valor_ingresado = readline("Ingrese algo:");

		contador = strlen(valor_ingresado);

		if(contador < tam){
			log_warning(logger_IO, "El texto debe ser mas largo");
		} else if (contador > tam){
			log_warning(logger_IO, "El texto debe ser mas corto");
		}

	}

	// ACA HAY QUE MANDAR A MEMORIA! Que memoria haga todo lo que tiene que hacer, que vuelva a 
	// mandar el OK a io, y ahi io manda el ok a kernel para pasar de block a ready!

	// t_buffer* buffer_a_enviar = crear_buffer();
    //  agregar_buffer_uint32(buffer_a_enviar,dir_fisica);
    //  agregar_buffer_string(buffer_a_enviar,valor_Ingresado;
	//  if(tam_reg8 != 0){
	//  	agregar_buffer_uint32(buffer_a_enviar,8);
	//  } else{
	//  	agregar_buffer_uint32(buffer_a_enviar,32);
	//  }

    // enviar_buffer(buffer_a_enviar,fd_kernel);
    // destruir_buffer(buffer_a_enviar);
}
