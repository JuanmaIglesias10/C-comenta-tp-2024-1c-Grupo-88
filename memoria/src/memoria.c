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
	char* nombreArchivo = leer_buffer_string(bufferMemoria);
	destruir_buffer(bufferMemoria);
	//Combino el path del .txt con el path del config
	char* rutaArchivo = string_new();
	string_append(&rutaArchivo, config_memoria.path_instrucciones); //     /home/utnso/scripts-pruebas/
	string_append(&rutaArchivo, nombreArchivo); // /home/utnso/scripts-pruebas/ + hola.txt

	t_list* listaInstrucciones = obtenerInstrucciones(rutaArchivo);
}

t_list* obtenerInstrucciones(char* rutaArchivo){
    //Creo mi lista de instrucciones
    t_list* listaInstrucciones;

	FILE* archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        log_info(logger_memoria , "Error al abrir el archivo");
        return 1;
    }
    /*      EJEMPLO DE ARCHIVOS DE INSTRUCCIONES     */
                // SET AX 03 
                // SUM BX 04
                // JNZ CX 04
                // RESIZE  BX   06   NULL
                //    ^     ^    ^
                // codIn   Par  Par   
    char linea[100];
    char* parametros;
    t_instruccion* instruccion;
    while (fgets(linea, sizeof(linea), archivo)) {
		// SET AX 03
    	strtok(linea, "\n");

        instruccion = malloc(sizeof(t_instruccion));
        instruccion->par1 = NULL;
        instruccion->par2 = NULL;
        instruccion->par3 = NULL;
        instruccion->par4 = NULL;
        instruccion->par5 = NULL;
        log_info(logger_memoria, "no exploté");
        // SET
        parametros = strtok(linea, " ");
        instruccion->codigo = obtenerCodigoInstruccion(parametros);

        //Seteo mi cantidad de parametros (maximo 5)
        int numParametro = 1;
        //AX
        parametros = strtok(NULL, " ");
		while(parametros != NULL){
			obtenerParametrosInstruccion(numParametro, instruccion, parametros);
			numParametro++;
			//03
			parametros = strtok(NULL, " ");
		}
		list_add(listaInstrucciones, instruccion);
    }

    fclose(archivo);
    return listaInstrucciones;
}

t_codigo_instruccion obtenerCodigoInstruccion(char* charParametro){
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
}

void obtenerParametrosInstruccion(int numParametro, t_instruccion* instrucciones, char* parametros){
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