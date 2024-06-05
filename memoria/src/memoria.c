#include "memoria.h"

int main(void) {
    inicializar_memoria();
    // Mantener el main en ejecución
    while (1) {
        sleep(1);
    }
    return 0;
}

void inicializar_memoria(){
    logger_memoria = iniciar_logger("logMemoria.log", "MEMORIA", LOG_LEVEL_INFO);
    inicializar_config();
    listaProcesos = list_create();

    pthread_mutex_init(&mutex_lista_procesos, NULL);

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
    if (fd_cpu == -1) {
        log_error(logger_memoria, "Error al esperar conexión de CPU");
        return;
    }

    fd_kernel = esperar_cliente(fd_memoria, logger_memoria, "KERNEL");
    if (fd_kernel == -1) {
        log_error(logger_memoria, "Error al esperar conexión de Kernel");
        return;
    }

    pthread_t hilo_memoria_cpu;
    if (pthread_create(&hilo_memoria_cpu, NULL, (void*)atender_cpu, NULL) != 0) {
        log_error(logger_memoria, "Error al crear hilo de CPU");
        return;
    }
    pthread_detach(hilo_memoria_cpu);

    pthread_t hilo_memoria_kernel;
    if (pthread_create(&hilo_memoria_kernel, NULL, (void*)atender_kernel, NULL) != 0) {
        log_error(logger_memoria, "Error al crear hilo de Kernel");
        return;
    }
    pthread_detach(hilo_memoria_kernel);

	while (1) {
	    int fd_IO = esperar_cliente(fd_memoria, logger_memoria, "IO");
	        if (fd_IO != -1) {
	            pthread_t hilo_memoria_IO;
	            int* fd_IO_ptr = malloc(sizeof(int)); // Alocamos memoria para el descriptor de archivo
	            *fd_IO_ptr = fd_IO;
	            if (pthread_create(&hilo_memoria_IO, NULL, (void*)atender_IO, fd_IO_ptr) != 0) {
	                log_error(logger_memoria, "Error al crear hilo de I/O");
	                free(fd_IO_ptr); // Liberamos memoria en caso de error
	            } else {
	                pthread_detach(hilo_memoria_IO);
	            }
	        } else {
	            log_info(logger_memoria, "No se recibió conexión de I/O");
	        }
	}
}



void iniciar_proceso(){
	//Recibo el buffer 
	t_buffer* bufferMemoria = recibir_buffer(fd_kernel);
	//Recibo el PID y el path del .txt
	uint32_t pid = leer_buffer_uint32(bufferMemoria);
	char* nombreArchivoInstrucciones = leer_buffer_string(bufferMemoria);
	destruir_buffer(bufferMemoria);
	//Combino el path del .txt con el path del config
	char* rutaArchivoInstrucciones = string_new();
	string_append(&rutaArchivoInstrucciones, config_memoria.path_instrucciones); 
	string_append(&rutaArchivoInstrucciones, nombreArchivoInstrucciones);
	t_list* listaInstrucciones = obtener_instrucciones(rutaArchivoInstrucciones);
	
	t_proceso* procesoNuevo = crear_proceso(listaInstrucciones, pid, 0);

	log_info(logger_memoria, "PID: %d - Tamaño: 0", pid); //LOG OBLIGATORIO, NO QUITAR!!!!!!

	pthread_mutex_lock(&mutex_lista_procesos);
	list_add(listaProcesos, procesoNuevo);
	pthread_mutex_unlock(&mutex_lista_procesos);
	
	//Todo ok -> Mando confirmacion a kernel 
	enviar_codOp(fd_kernel,INICIAR_PROCESO_OK);
	
	// list_destroy_and_destroy_elements(listaInstrucciones, (void*)instrucciones_destroy);  
	//Al hacer esto, libero las instrucciones en proceso, aunque ya las pasé en la linea 90 ,1 hora debuggeando para encontrar esta mrd :D
	free(rutaArchivoInstrucciones);
}

t_proceso* crear_proceso(t_list* listaInstrucciones, uint32_t pid, uint32_t tamanio){
	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->instrucciones = listaInstrucciones;
	proceso->pid = pid;
	proceso->cantMaxMarcos = tamanio / config_memoria.tam_pagina;

	return proceso;
}

t_list* obtener_instrucciones(char* pathArchivo){
    t_list* listaInstrucciones = list_create();

	FILE* archivo = fopen(pathArchivo, "r");
    if (archivo == NULL) {
        log_info(logger_memoria , "Error al abrir el archivo");
        return NULL;
    }
    char linea[100];
    char* parametros;
    t_instruccion* instruccion;
    while (fgets(linea, sizeof(linea), archivo)) {
    	strtok(linea, "\n");

        instruccion = malloc(sizeof(t_instruccion));
        instruccion->par1 = NULL;
        instruccion->par2 = NULL;
        instruccion->par3 = NULL;
        instruccion->par4 = NULL;
        instruccion->par5 = NULL;

        parametros = strtok(linea, " ");
        instruccion->codigo = obtener_codigo_instruccion(parametros);

        int numParametro = 1;
        parametros = strtok(NULL, " ");
		while(parametros != NULL){
			obtener_parametros_instruccion(numParametro, instruccion, parametros);
			numParametro++;
			parametros = strtok(NULL, " ");
		}
		list_add(listaInstrucciones, instruccion);
    }

    fclose(archivo);
    return listaInstrucciones;
}

t_codigo_instruccion obtener_codigo_instruccion(char* charParametro){
    if(strcmp(charParametro,"SET") == 0)
		return SET;
	if(strcmp(charParametro,"MOV_IN") == 0)
		return MOV_IN;
	if(strcmp(charParametro,"MOV_OUT") == 0)
		return MOV_OUT;
	if(strcmp(charParametro,"SUM") == 0)
		return SUM;
	if(strcmp(charParametro,"SUB") == 0)
		return SUB;
	if(strcmp(charParametro,"JNZ") == 0)
		return JNZ;
	if(strcmp(charParametro,"RESIZE") == 0)
		return RESIZE;
	if(strcmp(charParametro,"COPY_STRING") == 0)
		return COPY_STRING;
	if(strcmp(charParametro,"WAIT") == 0)
		return WAIT;
	if(strcmp(charParametro,"SIGNAL") == 0)
		return SIGNAL;
	if(strcmp(charParametro,"IO_GEN_SLEEP") == 0)
		return IO_GEN_SLEEP;
	if(strcmp(charParametro,"IO_STDIN_READ") == 0)
		return IO_STDIN_READ;
	if(strcmp(charParametro,"IO_STDOUT_WRITE") == 0)
		return IO_STDOUT_WRITE;
	if(strcmp(charParametro,"IO_FS_CREATE") == 0)
		return IO_FS_CREATE;
	if(strcmp(charParametro,"IO_FS_DELETE") == 0)
		return IO_FS_DELETE;
	if(strcmp(charParametro,"IO_FS_TRUNCATE") == 0)
		return IO_FS_TRUNCATE;
    if(strcmp(charParametro,"IO_FS_WRITE") == 0)
        return IO_FS_WRITE;
    if(strcmp(charParametro,"IO_FS_READ") == 0)
        return IO_FS_READ;
    if(strcmp(charParametro,"EXIT") == 0)
        return EXIT;
	else{
		return 100; //TODO
	}
}

void obtener_parametros_instruccion(int numParametro, t_instruccion* instrucciones, char* parametros){
    //Obtengo el tamaño del char* y le añado +1 por el \0
	int tam = strlen(parametros) + 1;
    //Dependiendo del numPar (incrementando en el while), lo asigno a su respectivo instrucciones->par{numero}
	switch(numParametro){
		case 1:
			instrucciones->par1 = malloc(tam);
			strcpy(instrucciones->par1, parametros);
			string_append(&(instrucciones->par1), "\0");
			break;
		case 2:
			instrucciones->par2 = malloc(tam);
			strcpy(instrucciones->par2, parametros);
			string_append(&(instrucciones->par2), "\0");
			break;
		case 3:
			instrucciones->par3 = malloc(tam);
			strcpy(instrucciones->par3, parametros);
			string_append(&(instrucciones->par3), "\0");
			break;
        case 4:
			instrucciones->par4 = malloc(tam);
			strcpy(instrucciones->par4, parametros);
			string_append(&(instrucciones->par4), "\0");
			break;
        case 5:
			instrucciones->par5 = malloc(tam);
			strcpy(instrucciones->par5, parametros);
			string_append(&(instrucciones->par5), "\0");
			break;
		}
}

//Esto caga todo :D lpm
void instrucciones_destroy(t_instruccion* instrucciones_a_destruir){
	// free(script_a_destruir->codigo_operacion);
	free(instrucciones_a_destruir->par1);
	free(instrucciones_a_destruir->par2);
	free(instrucciones_a_destruir->par3);
	free(instrucciones_a_destruir->par4);
	free(instrucciones_a_destruir->par5);
	free(instrucciones_a_destruir);
	return;
}

void enviar_instruccion(){
	t_buffer* buffer = recibir_buffer(fd_cpu);

	uint32_t pid = leer_buffer_uint32(buffer);
	uint32_t pc = leer_buffer_uint32(buffer);

	destruir_buffer(buffer);
	// Suponemos que si se consulta por un proceso es porque ya existe

	pthread_mutex_lock(&mutex_lista_procesos);
	t_proceso* proceso = buscarProcesoPorPid(pid);
	pthread_mutex_unlock(&mutex_lista_procesos);

	t_instruccion* instruccion = list_get(proceso->instrucciones, pc);
	buffer = crear_buffer();
	agregar_buffer_instruccion(buffer, instruccion);
	enviar_buffer(buffer, fd_cpu);
	/* Para probar que se manden las instrucciones, NO BORRAR!
	*/
    log_info(logger_memoria,"%d %s %s %s %s %s",instruccion->codigo,instruccion->par1,instruccion->par2,instruccion->par4,instruccion->par4,instruccion->par5);
	destruir_buffer(buffer);
}

t_proceso* buscarProcesoPorPid(uint32_t pid){
	for(int i = 0; i < list_size(listaProcesos); i++){
		t_proceso* proceso = list_get(listaProcesos, i);
		if(proceso->pid == pid)
			return proceso;
	}
	return NULL; 
}