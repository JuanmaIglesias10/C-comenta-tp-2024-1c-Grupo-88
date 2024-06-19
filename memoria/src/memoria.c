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

    pthread_mutex_init(&mutex_lista_procesos, NULL);

	inicializar_variables();

    inicializar_conexiones();


    return;
}

void inicializar_config(){
    config = config_create("./memoria.config");
    config_memoria.puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    config_memoria.tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    config_memoria.tam_pagina = config_get_int_value(config, "TAM_PAGINA");
    config_memoria.path_instrucciones = config_get_string_value(config, "PATH_INSTRUCCIONES");
    config_memoria.retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");
}

void inicializar_variables(){
	listaProcesos = list_create();
	listaMarcos = list_create();
	tablaGlobalPaginas = list_create();

	// sem_init(&finalizacion, 0, 0);
	// sem_init(&sem_pagina_cargada, 0, 0);

	cantMarcos = config_memoria.tam_memoria / config_memoria.tam_pagina; //128
	
	for(int i = 0; i < cantMarcos; i++)
		list_add(listaMarcos, NULL); //Inicializo la lista de marcos

	for(int i = 0; i < cantMarcos; i++)
		list_add(tablaGlobalPaginas, NULL); //Inicializo la lista de marcos



	memoriaPrincipal = malloc(config_memoria.tam_memoria);
	
	// indicePorFifo = 0;
}

void inicializar_conexiones() {
    fd_memoria = iniciar_servidor(config_memoria.puerto_escucha, logger_memoria);

    fd_cpu = esperar_cliente(fd_memoria, logger_memoria, "CPU");
    if (fd_cpu == -1) {
        log_error(logger_memoria, "Error al esperar conexión de CPU");
        return;
    }

	enviar_tamanio_pagina();

    fd_kernel = esperar_cliente(fd_memoria, logger_memoria, "KERNEL");
    if (fd_kernel == -1) {
        log_error(logger_memoria, "Error al esperar conexión de Kernel");
        return;
    }

	pthread_t hilo_IO_accept;
    pthread_create(&hilo_IO_accept, NULL, (void*)aceptar_conexiones_IO, (void*)&fd_memoria);
    pthread_detach(hilo_IO_accept);

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

}

void enviar_tamanio_pagina(){
	t_buffer* buffer = crear_buffer();
	agregar_buffer_uint32(buffer, config_memoria.tam_pagina);
	enviar_buffer(buffer, fd_cpu);
	destruir_buffer(buffer);
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
	
	t_proceso* procesoNuevo = crear_proceso(listaInstrucciones, pid);

	log_info(logger_memoria, "PID: %d - Tamaño: %d", procesoNuevo->pid , procesoNuevo->cantPaginas); //Este log se repite en terminar_proceso

	pthread_mutex_lock(&mutex_lista_procesos);
	list_add(listaProcesos, procesoNuevo);
	pthread_mutex_unlock(&mutex_lista_procesos);
	
	//Todo ok -> Mando confirmacion a kernel 
	enviar_codOp(fd_kernel,INICIAR_PROCESO_OK);
	
	// list_destroy_and_destroy_elements(listaInstrucciones, (void*)instrucciones_destroy);  
	//Al hacer esto, libero las instrucciones en proceso, aunque ya las pasé en la linea 90 ,1 hora debuggeando para encontrar esta mrd :D
	free(rutaArchivoInstrucciones);

}

t_proceso* crear_proceso(t_list* listaInstrucciones, uint32_t pid){
	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->instrucciones = listaInstrucciones;
	proceso->pid = pid;
	proceso->cantPaginas = 0 ; //El tamaño arranca en 0 y se modifica con resize
	proceso->tamaño = 0;

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

//TODO: Testear el liberar las Instrucciones
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
    log_info(logger_memoria,"%d %s %s %s %s %s",instruccion->codigo,instruccion->par1,instruccion->par2,instruccion->par4,instruccion->par4,instruccion->par5);
	*/
	destruir_buffer(buffer);
}

void ejecutar_MOV_IN(){
	t_buffer* buffer = recibir_buffer(fd_cpu); 
	uint32_t dirFisica = leer_buffer_uint32(buffer);
	uint32_t pid = leer_buffer_uint32(buffer);
	uint32_t nroPag = leer_buffer_uint32(buffer);
	destruir_buffer(buffer);

	uint32_t valorLeido = 0;

	memcpy(&valorLeido, memoriaPrincipal + dirFisica, sizeof(uint32_t));

	enviar_codOp(fd_cpu, MOV_IN_OK); //Hacia ejecutar_mov_in en instrucciones.c

	t_pagina* pagLeida = buscarPaginaPorNroYPid(nroPag, pid);
	pagLeida->ultimaReferencia = temporal_get_string_time("%H:%M:%S:%MS"); //LRU

	log_info(logger_memoria, "PID: %d - Acción: LEER - Dirección física: %d", pid, dirFisica);

	//log_warning(logger_memoria, "Valor leido: %d", valorLeido);
	buffer = crear_buffer();
	agregar_buffer_uint32(buffer, valorLeido);
	enviar_buffer(buffer, fd_cpu);
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

void devolver_nro_marco(){
	t_buffer* buffer = recibir_buffer(fd_cpu); //Desde calcular_direccion_fisica en mmu.c
	uint32_t nro_pagina = leer_buffer_uint32(buffer);
	uint32_t pid = leer_buffer_uint32(buffer);
	destruir_buffer(buffer);
	
	t_pagina* pagina = buscarPaginaPorNroYPid(nro_pagina, pid);
	
	
	enviar_codOp(fd_cpu, NUMERO_MARCO_OK);  //Hacia calcular_direccion_fisica en mmu.c
	buffer = crear_buffer();
	agregar_buffer_uint32(buffer, pagina->nroMarco);
	enviar_buffer(buffer, fd_cpu);
	destruir_buffer(buffer);
}

t_pagina* buscarPaginaPorNroYPid(uint32_t nroPag, uint32_t pid){

	for(int i = 0; i < list_size(tablaGlobalPaginas); i++){

		t_pagina* pag = list_get(tablaGlobalPaginas, i);

		if(pag->nroPagina == nroPag && pag->pidProcesoCreador == pid)
			return pag;
	}

	return NULL;
}



void resize() {
	t_buffer* buffer = recibir_buffer(fd_cpu);
	uint32_t pid     = leer_buffer_uint32(buffer);
	uint32_t nuevoTamaño  = leer_buffer_uint32(buffer);
	destruir_buffer(buffer);

	t_proceso* proceso = buscarProcesoPorPid(pid);

	uint32_t cantMarcosNecesitados = ceil(nuevoTamaño / config_memoria.tam_pagina);

	if(hay_marcos_libres(cantMarcosNecesitados)){ //Tengo marcos disponibles, avanzo con el resize

		if(nuevoTamaño > proceso->tamaño){ //Ampliacion del tamaño del proceso
			log_info(logger_memoria,"PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d", proceso->pid, proceso->tamaño , nuevoTamaño);
			proceso->cantPaginas = ceil(nuevoTamaño / config_memoria.tam_pagina);
			
			while(cantMarcosNecesitados > 0){
				
				uint32_t nroMarcoLibre = obtener_marco_libre();
				uint32_t nroPaginaLibre = obtener_pagina_libre();

				//La direccion de inicio es void* porque memoriaPrincipal lo es
				void* direccionInicioPagina = memoriaPrincipal + nroMarcoLibre * config_memoria.tam_pagina;

				t_pagina* paginaCreada = crear_pagina(nroPaginaLibre, nroMarcoLibre, direccionInicioPagina, proceso->pid);

				list_replace(listaMarcos, nroMarcoLibre, paginaCreada); // Coloco la pagina en el marco libre

				cantMarcosNecesitados--;
			}
			proceso->tamaño = nuevoTamaño;
		} else if (nuevoTamaño < proceso->tamaño){ //Reduccion del tamaño del proceso
			log_info(logger_memoria,"PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d", proceso->pid, proceso->tamaño , nuevoTamaño);
			liberarPaginasDeUnProcesoResize(proceso, nuevoTamaño);
			proceso->cantPaginas = ceil(nuevoTamaño / config_memoria.tam_pagina);
			proceso->tamaño = nuevoTamaño;
		} else { //El resize es innecesario, el tamaño es el mismo
			log_info(logger_memoria, "No es necesario realizar una ampliacion o reduccion de tamaño");
		}
		enviar_codOp(fd_cpu, RESIZE_OK);	
	} else {
		enviar_codOp(fd_cpu, OUT_OF_MEMORY);
	}

}

t_pagina* crear_pagina(uint32_t nroPag, uint32_t nroMarco, void* dirreccionInicio, uint32_t pid){
	t_pagina* paginaCreada= malloc(sizeof(t_pagina));

	paginaCreada->bitModificado = false; //--------------------------- Recordar esto
	paginaCreada->bitPresencia = true;
	paginaCreada->direccionFisicaInicio = dirreccionInicio;
	paginaCreada->nroMarco = nroMarco;
	paginaCreada->nroPagina = nroPag;
	paginaCreada->pidProcesoCreador = pid;
	paginaCreada->ultimaReferencia = temporal_get_string_time("%H:%M:%S:%MS");

	list_replace(tablaGlobalPaginas, nroPag , paginaCreada);

	return paginaCreada;
}

void colocar_pagina_en_marco(uint32_t pid, uint32_t nroPagina){
	uint32_t numeroMarco = obtener_marco_libre();

	uint32_t posEnMemoria =  numeroMarco * config_memoria.tam_pagina;

	t_pagina* pagNueva = crear_pagina(nroPagina, numeroMarco, memoriaPrincipal + posEnMemoria, pid);
	nroPagina++;
	list_replace(listaMarcos, numeroMarco, pagNueva);

	escribir_pagina(posEnMemoria, pagNueva);
	
	pagNueva->bitModificado = false;
	pagNueva->bitPresencia = true;
	pagNueva->direccionFisicaInicio = memoriaPrincipal + posEnMemoria;
	pagNueva->pidProcesoCreador = pid;
	pagNueva->nroMarco = numeroMarco;

	
	// sem_post(&sem_pagina_cargada);
}

bool hay_marcos_libres(int cantMarcosNecesitada){ //
	int j = 0;
	for(int i = 0; i < cantMarcos; i++){

		t_pagina* pagina = list_get(listaMarcos, i);//no vacia - no vacia - vacia - vacia - vacia
		if(pagina == NULL) //la pagina esta vacia
			j++; //aumento el contador
	}
	return (j >= cantMarcosNecesitada);
}

uint32_t obtener_marco_libre(){
	for(int i = 0; i < cantMarcos; i++){

		t_pagina* pagina = list_get(listaMarcos, i);
		if(pagina == NULL)
			return i;
	}
	return -1;
}

uint32_t obtener_pagina_libre(){
	for(int i = 0; i < cantMarcos; i++){
		t_pagina* pagina = list_get(tablaGlobalPaginas, i);
		if(pagina == NULL)
			return i;
	}
	return -1;
}

void escribir_pagina(uint32_t posEnMemoria, void* pagina){
	memcpy(memoriaPrincipal + posEnMemoria, pagina, config_memoria.tam_pagina);
}


void liberarPaginasDeUnProcesoResize(t_proceso* procesoAReducir, uint32_t nuevoTamaño){
	int cantPaginasAEliminar = ceil((procesoAReducir->tamaño - nuevoTamaño)/config_memoria.tam_pagina);

	for(int j = cantMarcos - 1; j >= 0; j--){
			
			t_pagina* pagina = list_get(tablaGlobalPaginas, j);
			if(pagina != NULL){
				if(pagina->pidProcesoCreador == procesoAReducir->pid){
					vaciar_marco(pagina->nroMarco);
					list_remove(tablaGlobalPaginas, j);
					free(pagina);
					cantPaginasAEliminar--;
					if(cantPaginasAEliminar == 0){
						break;
					}
				}
			}
		}	
}

void vaciar_marco(uint32_t nroMarco){
	list_replace(listaMarcos, nroMarco, NULL);
}



// log_info(logger_memoria, "Destrucción: PID: %d - Tamaño: %d", procesoAEliminar->pid, cantPaginas);