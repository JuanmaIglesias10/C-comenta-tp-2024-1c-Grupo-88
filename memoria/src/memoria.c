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
	FILE* archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    char linea[100];

    t_instruccion instruccion;
    while (fgets(linea, sizeof(linea), archivo)) {
		//Obtengo la primer linea del archivo y avanza para la siguiente iteracion del while
        char* token = strtok(linea, " \n");



        if (strcmp(token, "SET") == 0)
            instruccion.codigo = SET;
        else if (strcmp(token, "SUM") == 0)
            instruccion.codigo = SUM;
        else if (strcmp(token, "SUB") == 0)
            instruccion.codigo = SUB;
        // Agrega más casos para cada valor del enum según sea necesario

        token = strtok(NULL, " \n");
        instruccion.par1 = strdup(token);

        token = strtok(NULL, " \n");
        instruccion.par2 = strdup(token);

        token = strtok(NULL, " \n");
        if (token != NULL) {
            instruccion.par3 = strdup(token);
        } else {
            instruccion.par3 = NULL; // Si par3 no está presente, asigna NULL
        }
        // Aquí puedes realizar cualquier operación que necesites con la instrucción
        // Por ejemplo, imprimir los valores almacenados en la estructura
        printf("Código: %d, Par1: %s, Par2: %s, Par3: %s\n",
               instruccion.codigo, instruccion.par1,
               instruccion.par2, instruccion.par3);

        // Libera la memoria asignada dinámicamente para evitar fugas de memoria
        free(instruccion.par1);
        free(instruccion.par2);
        free(instruccion.par3);
    }

    fclose(archivo);
    return 0;
}