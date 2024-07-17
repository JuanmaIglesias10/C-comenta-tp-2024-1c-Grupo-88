#include "memoria.h"

int main(void) {
    inicializar_memoria();
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

	// sem_init(&finalizacion, 0, 0);
	// sem_init(&sem_pagina_cargada, 0, 0);

	cantMarcos = config_memoria.tam_memoria / config_memoria.tam_pagina; //128
	
	for(int i = 0; i < cantMarcos; i++)
		list_add(listaMarcos, NULL); //Inicializo la lista de marcos

	memoriaPrincipal = malloc(config_memoria.tam_memoria);
	memset(memoriaPrincipal, 0, config_memoria.tam_memoria);
	
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
	if (pthread_create(&hilo_IO_accept, NULL, (void*)aceptar_conexiones_IO, (void*)&fd_memoria) != 0){
        log_error(logger_memoria, "Error al crear hilo de IO");
        return;
    }
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
    pthread_join(hilo_memoria_kernel, NULL);

}

void enviar_tamanio_pagina(){
	t_buffer* buffer = crear_buffer();
	agregar_buffer_uint32(buffer, config_memoria.tam_pagina);
	enviar_buffer(buffer, fd_cpu);
	destruir_buffer(buffer);
	}

void iniciar_proceso(){
	// Recibo el PID y el path del .txt
	t_buffer* bufferMemoria = recibir_buffer(fd_kernel);
	uint32_t pid = leer_buffer_uint32(bufferMemoria);
	char* nombreArchivoInstrucciones = leer_buffer_string(bufferMemoria);
	destruir_buffer(bufferMemoria);

	// Combino el path del .txt con el path del config
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

	free(rutaArchivoInstrucciones);
	free(nombreArchivoInstrucciones);

}

t_proceso* crear_proceso(t_list* listaInstrucciones, uint32_t pid){
	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->instrucciones = listaInstrucciones;
	proceso->pid = pid;
	proceso->cantPaginas = 0 ; //El tamaño arranca en 0 y se modifica con resize
	proceso->tamaño = 0;
	proceso->listaPaginasProceso = list_create(); //Tabla de Paginas del Proceso

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
	free(parametros);
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

void ejecutar_MOV_OUT(){
	t_buffer* buffer = recibir_buffer(fd_cpu);
	uint32_t dirFisica = leer_buffer_uint32(buffer);
	uint32_t tamValor = leer_buffer_uint32(buffer);
	uint8_t valorAEscribir8;
	uint32_t valorAEscribir32;
	if(tamValor == 8){
		valorAEscribir8 = leer_buffer_uint8(buffer);
	} else if (tamValor == 32){
		valorAEscribir32 = leer_buffer_uint32(buffer);
	}	
	uint32_t pid = leer_buffer_uint32(buffer);
	uint32_t numPagina = leer_buffer_uint32(buffer);
	destruir_buffer(buffer);

	t_proceso* proceso  = buscarProcesoPorPid(pid);
	
	if(tamValor == 8){
    	// escribirValorEnMemoria(memoriaPrincipal, valorAEscribir8, dirFisica, 1, proceso);
        memcpy(memoriaPrincipal + dirFisica, &valorAEscribir8, 1);

	} else if (tamValor == 32){
   		 escribirValorEnMemoria(valorAEscribir32, dirFisica, 4, proceso);
	}


	enviar_codOp(fd_cpu, MOV_OUT_OK);
	// imprimir_memoria();

}

void escribirValorEnMemoria(uint32_t valor, size_t dirFisica, int cantBytes, t_proceso* proceso){
    uint8_t bytesValor[cantBytes];
    memcpy(bytesValor, &valor, cantBytes);

    size_t bytesRestantes = cantBytes;
    size_t bytesEscritos = 0;

    // Inicializar el primer marco y el offset inicial
    size_t offsetActual = dirFisica % config_memoria.tam_pagina;
	size_t nroMarcoActual  = (dirFisica - offsetActual)/config_memoria.tam_pagina;
	uint32_t siguientePagina = 0;
	for (size_t i = 0; i < list_size(proceso->listaPaginasProceso); i++)
	{
    	t_pagina* pagina = list_get(proceso->listaPaginasProceso, i);
		if(pagina->nroMarco == nroMarcoActual){
			siguientePagina = i + 1;
			if(siguientePagina == list_size(proceso->listaPaginasProceso)){
				siguientePagina = 0;
			}
		}
	}
	
    while (bytesRestantes > 0) {
        // Calcular la cantidad de bytes a escribir en el marco actual
        size_t espacioDisponible = config_memoria.tam_pagina - offsetActual;
        size_t bytesAEescribir = (bytesRestantes < espacioDisponible) ? bytesRestantes : espacioDisponible;

        // Escribir los bytes correspondientes en el marco actual
        memcpy(memoriaPrincipal + nroMarcoActual * config_memoria.tam_pagina + offsetActual, bytesValor + bytesEscritos, bytesAEescribir);

        // Actualizar contadores

		t_pagina* pagina = list_get(proceso->listaPaginasProceso, siguientePagina);
		nroMarcoActual = pagina->nroMarco;

		siguientePagina++;
		if(siguientePagina == list_size(proceso->listaPaginasProceso)){
			siguientePagina = 0;
		}
        bytesRestantes -= bytesAEescribir;
        bytesEscritos += bytesAEescribir;

        // Pasar al siguiente marco y reiniciar el offset
        offsetActual = 0;  // Después del primer marco, el offset es 0
    }
}

void ejecutar_MOV_IN(){
	uint32_t tamanio;
	uint32_t dirFisica;
	uint32_t pid;
	uint32_t nroPag;
	
	t_buffer* buffer = recibir_buffer(fd_cpu);

	tamanio = leer_buffer_uint32(buffer);
	dirFisica = leer_buffer_uint32(buffer);
	pid = leer_buffer_uint32(buffer);
	nroPag = leer_buffer_uint32(buffer);

	destruir_buffer(buffer);

	// aca voy a guardar lo que lea en memoria
	uint8_t valorLeido8 = 0; 
	uint32_t valorLeido32 = 0;
	t_proceso* proceso  = buscarProcesoPorPid(pid);

	if (tamanio == 8) {
		memcpy(&valorLeido8, memoriaPrincipal + dirFisica, sizeof(uint8_t));
	} else if (tamanio == 32) { 
	  	valorLeido32 = leerValorEnMemoria(dirFisica, 4, proceso);
	} else {
		log_warning(logger_memoria, "tamanio invalido del valor a leer");
	}

	enviar_codOp(fd_cpu, MOV_IN_OK); // Lo recibe ejecutar_mov_in() en instrucciones.c en CPU
	log_info(logger_memoria, "PID: %d - Acción: LEER - Dirección física: %d", pid, dirFisica);
	buffer = crear_buffer();
	if (tamanio == 8) {
		agregar_buffer_uint8(buffer, valorLeido8);
	} else { 
		agregar_buffer_uint32(buffer, valorLeido32);
	}
	enviar_buffer(buffer, fd_cpu);
	destruir_buffer(buffer);
}

uint32_t leerValorEnMemoria(size_t dirFisica, int cantBytes, t_proceso* proceso){
	void* valorARetornar = malloc(cantBytes); 

    size_t bytesRestantesPorLeer = cantBytes;
    size_t bytesLeidos = 0;

    // Inicializar el primer marco y el offset inicial
    size_t offsetActual = dirFisica % config_memoria.tam_pagina;
	size_t nroMarcoActual  = (dirFisica - offsetActual)/config_memoria.tam_pagina;
	uint32_t siguientePagina = 0;
	for (size_t i = 0; i < list_size(proceso->listaPaginasProceso); i++)
	{
    	t_pagina* pagina = list_get(proceso->listaPaginasProceso, i);
		if(pagina->nroMarco == nroMarcoActual){
			siguientePagina = i + 1;
			if(siguientePagina == list_size(proceso->listaPaginasProceso)){
				siguientePagina = 0;
			}
		}
	}
	
    while (bytesRestantesPorLeer > 0) {
        // Calcular la cantidad de bytes a escribir en el marco actual
        size_t espacioDisponible = config_memoria.tam_pagina - offsetActual;
        size_t bytesALeer = (bytesRestantesPorLeer < espacioDisponible) ? bytesRestantesPorLeer : espacioDisponible;

        memcpy(valorARetornar + bytesLeidos, memoriaPrincipal + nroMarcoActual * config_memoria.tam_pagina + offsetActual, bytesALeer);

		t_pagina* pagina = list_get(proceso->listaPaginasProceso, siguientePagina);
		nroMarcoActual = pagina->nroMarco;

		siguientePagina++;
		if(siguientePagina == list_size(proceso->listaPaginasProceso)){
			siguientePagina = 0;
		}
        bytesRestantesPorLeer -= bytesALeer;
        bytesLeidos += bytesALeer;

        offsetActual = 0;  // Después del primer marco, el offset es 0
    }
	uint32_t* uint32_valorARetornar = (uint32_t*)valorARetornar;
    uint32_t valor = *uint32_valorARetornar;
	return valor;
}

t_proceso* buscarProcesoPorPid(uint32_t pid){
	for(int i = 0; i < list_size(listaProcesos); i++){

		t_proceso* proceso = list_get(listaProcesos, i);
		if(proceso->pid == pid)
			return proceso;
	}
	return NULL; 
}

// Luego de recibir el codOp: NUMERO_MARCO_SOLICITUD - desde CPU
void devolver_nro_marco(){
	t_buffer* buffer = recibir_buffer(fd_cpu); //Desde calcular_direccion_fisica en mmu.c
	uint32_t nro_pagina = leer_buffer_uint32(buffer);
	uint32_t pid = leer_buffer_uint32(buffer);
	destruir_buffer(buffer);
	t_proceso* proceso = buscarProcesoPorPid(pid);

	t_pagina* pagina = buscarPaginaPorNroYPid(proceso,nro_pagina);
	
	enviar_codOp(fd_cpu, NUMERO_MARCO_OK);  //Hacia calcular_direccion_fisica en mmu.c
	buffer = crear_buffer();
	agregar_buffer_uint32(buffer, pagina->nroMarco);
	enviar_buffer(buffer, fd_cpu);
	destruir_buffer(buffer);
}

t_pagina* buscarPaginaPorNroYPid(t_proceso* proceso , uint32_t nroPag){
	for(int i = 0; i < list_size(proceso->listaPaginasProceso); i++){
		t_pagina* pag = list_get(proceso->listaPaginasProceso, i);
		if(pag->nroPagina == nroPag)
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

	int nro_pagina_nueva = proceso->cantPaginas;
	/*
	 Inicializo el contador en la cant de paginas anterior
	 Si proceso->CantPaginas tenia 4 paginas, entonces voy a empezar a añadir en la tabla a partir de la pagina 4 (0,1,2,3 ya estan ocupadas)
	*/
	uint32_t cantMarcosNecesitados = (uint32_t)ceil((double)nuevoTamaño / config_memoria.tam_pagina) - proceso->cantPaginas;

	if(hay_marcos_libres(cantMarcosNecesitados) || nuevoTamaño < proceso->tamaño){ //Tengo marcos disponibles, avanzo con el resize

		if(nuevoTamaño > proceso->tamaño){ //Ampliacion del tamaño del proceso
			log_info(logger_memoria,"PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d", proceso->pid, proceso->tamaño , nuevoTamaño);
			proceso->cantPaginas = (uint32_t)ceil((double)nuevoTamaño / config_memoria.tam_pagina); //Son lo mismo
			
			while(cantMarcosNecesitados > 0){
	
				uint32_t nroMarcoLibre = obtener_marco_libre();
				// uint32_t nroPaginaLibre = obtener_pagina_libre(); HAY QUE SACARLA

				void* direccionInicioPagina = memoriaPrincipal + nroMarcoLibre * config_memoria.tam_pagina;

				t_pagina* paginaCreada = crear_pagina(nro_pagina_nueva, nroMarcoLibre, direccionInicioPagina, proceso->pid);
				log_warning(logger_memoria, "El PID %d con el nro de Pagina %d ocupa el marco %d", proceso->pid, paginaCreada->nroPagina, paginaCreada->nroMarco);
				list_add(proceso->listaPaginasProceso, paginaCreada);

				list_replace(listaMarcos , nroMarcoLibre ,paginaCreada);
				
				cantMarcosNecesitados--;

				nro_pagina_nueva++; 
			}
			proceso->tamaño = nuevoTamaño;
		} else if (nuevoTamaño < proceso->tamaño){ //Reduccion del tamaño del proceso
			log_info(logger_memoria,"PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d", proceso->pid, proceso->tamaño , nuevoTamaño);
			liberarPaginasDeUnProcesoResize(proceso, nuevoTamaño);
			proceso->cantPaginas = (uint32_t)ceil((double)nuevoTamaño / config_memoria.tam_pagina);
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
	paginaCreada->direccionFisicaInicio = dirreccionInicio;
	paginaCreada->nroMarco = nroMarco;
	paginaCreada->nroPagina = nroPag;
	paginaCreada->pidProcesoCreador = pid;

	return paginaCreada;
}

void colocar_pagina_en_marco(uint32_t pid, uint32_t nroPagina){
	uint32_t numeroMarco = obtener_marco_libre();

	uint32_t posEnMemoria =  numeroMarco * config_memoria.tam_pagina;

	t_pagina* pagNueva = crear_pagina(nroPagina, numeroMarco, memoriaPrincipal + posEnMemoria, pid);
	nroPagina++;
	list_replace(listaMarcos, numeroMarco, pagNueva);

	escribir_pagina(posEnMemoria, pagNueva);
	
	pagNueva->direccionFisicaInicio = memoriaPrincipal + posEnMemoria;
	pagNueva->pidProcesoCreador = pid;
	pagNueva->nroMarco = numeroMarco;

	
	// sem_post(&sem_pagina_cargada);
}

bool hay_marcos_libres(int cantMarcosNecesitada){ //129
	int j = 0;
	for(int i = 0; i < cantMarcos; i++){

		t_pagina* pagina = list_get(listaMarcos, i);//no vacia - no vacia - vacia - vacia - vacia
		if(pagina == NULL) //la pagina esta vacia
			j++; //aumento el contador
	}
	return (j >= cantMarcosNecesitada);
}

uint32_t obtener_marco_libre(){
	for(int i = 0; i < cantMarcos; i++){ //128
		if(list_get(listaMarcos, i) == NULL){
			return i;
		}
	}
	return -1;
}

void escribir_pagina(uint32_t posEnMemoria, void* pagina){
	memcpy(memoriaPrincipal + posEnMemoria, pagina, config_memoria.tam_pagina);
}

void liberarPaginasDeUnProcesoResize(t_proceso* procesoAReducir, uint32_t nuevoTamaño){
	uint32_t cantPaginasAEliminar = (uint32_t)ceil((double)(procesoAReducir->tamaño - nuevoTamaño)/config_memoria.tam_pagina);
	for(int j = list_size(procesoAReducir->listaPaginasProceso) - 1; j >= 0; j--){		
		t_pagina* pagina = list_get(procesoAReducir->listaPaginasProceso, j);
		vaciar_marco(pagina->nroMarco);
		list_remove(procesoAReducir->listaPaginasProceso, j);
		free(pagina);
		cantPaginasAEliminar--;
		if(cantPaginasAEliminar == 0){
			break;
		}
	}	
}

void vaciar_marco(uint32_t nroMarco){
	list_replace(listaMarcos, nroMarco, NULL);
}



// log_info(logger_memoria, "Destrucción: PID: %d - Tamaño: %d", procesoAEliminar->pid, cantPaginas);

// TODO: revisar esta funcion que esta mal
void escribir_stdin_read(int fd_IO){
    t_buffer* buffer_recibido = recibir_buffer(fd_IO);
    uint32_t dirFisica = leer_buffer_uint32(buffer_recibido);
	uint32_t pid = leer_buffer_uint32(buffer_recibido);
    char* valor_ingresado = leer_buffer_string(buffer_recibido);
    uint32_t tamanio = leer_buffer_uint32(buffer_recibido);
    destruir_buffer(buffer_recibido);
	t_proceso* proceso = buscarProcesoPorPid(pid);
	escribirValorEnMemoriaString(valor_ingresado, dirFisica, tamanio, proceso);
	
	enviar_codOp(fd_IO, IO_M_STDIN_READ_OK);
	// imprimir_memoria();
	free(valor_ingresado);
}

void leer_stdout_write(int fd_IO){
    t_buffer* buffer_recibido = recibir_buffer(fd_IO);
    uint32_t dirFisica 		  = leer_buffer_uint32(buffer_recibido);
	uint32_t pid 			  = leer_buffer_uint32(buffer_recibido);
    uint32_t tamanio          = leer_buffer_uint32(buffer_recibido);

    destruir_buffer(buffer_recibido);
	
	t_proceso* proceso = buscarProcesoPorPid(pid);
	
	char* string_leido = leerValorEnMemoriaString(dirFisica, tamanio, proceso);
	log_info(logger_memoria,"%s",string_leido);
	
	enviar_codOp(fd_IO, IO_M_STDOUT_WRITE_OK);
	t_buffer* buffer_a_enviar = crear_buffer();
	agregar_buffer_string(buffer_a_enviar, string_leido);
	enviar_buffer(buffer_a_enviar,fd_IO);
    destruir_buffer(buffer_a_enviar);
	free(string_leido);

	// imprimir_memoria();
}

void imprimir_memoria() {
    int nroMarcoVariable = 0;
    int i = 0;

    while (nroMarcoVariable * config_memoria.tam_pagina < config_memoria.tam_memoria) {
        printf("Marco %d:", nroMarcoVariable);
        for (i = 0; i < config_memoria.tam_pagina; i++) {
            unsigned char obtenido;
            memcpy(&obtenido, (unsigned char *)memoriaPrincipal + nroMarcoVariable * config_memoria.tam_pagina + i, 1);
            printf(" %02X", obtenido); // Imprimir cada byte en formato hexadecimal
        }
        printf("\n");
        nroMarcoVariable++;
    }
	log_warning(logger_memoria , "-------------------------------------------");
}

void escribirValorEnMemoriaString(char* valor, size_t dirFisica, int tamString, t_proceso* proceso) {
    // Convertir el string a bytes individuales
    uint8_t bytesValor[tamString];
    memcpy(bytesValor, valor, tamString);

    size_t bytesRestantes = tamString;
    size_t bytesEscritos = 0;

    size_t offsetActual = dirFisica % config_memoria.tam_pagina;
    size_t nroMarcoActual = (dirFisica - offsetActual) / config_memoria.tam_pagina;
    uint32_t siguientePagina = 0;

    // Iterar sobre las páginas del proceso para determinar la siguiente página
    for (size_t i = 0; i < list_size(proceso->listaPaginasProceso); i++) {
        t_pagina* pagina = list_get(proceso->listaPaginasProceso, i);
        if (pagina->nroMarco == nroMarcoActual) {
            siguientePagina = i + 1;
            if (siguientePagina == list_size(proceso->listaPaginasProceso)) {
                siguientePagina = 0;
            }
        }
    }

    while (bytesRestantes > 0) {
        // Calcular la cantidad de bytes a escribir en el marco actual
        size_t espacioDisponible = config_memoria.tam_pagina - offsetActual;
        size_t bytesAEscribir = (bytesRestantes < espacioDisponible) ? bytesRestantes : espacioDisponible;

        // Escribir los bytes correspondientes en el marco actual
        memcpy(memoriaPrincipal + nroMarcoActual * config_memoria.tam_pagina + offsetActual, bytesValor + bytesEscritos, bytesAEscribir);

        // Actualizar contadores
        t_pagina* pagina = list_get(proceso->listaPaginasProceso, siguientePagina);
        nroMarcoActual = pagina->nroMarco;

        siguientePagina++;
        if (siguientePagina == list_size(proceso->listaPaginasProceso)) {
            siguientePagina = 0;
        }

        bytesRestantes -= bytesAEscribir;
        bytesEscritos += bytesAEscribir;

        // Pasar al siguiente marco y reiniciar el offset
        offsetActual = 0;  // Después del primer marco, el offset es 0
    }
}

char* leerValorEnMemoriaString(size_t dirFisica, int tamString, t_proceso* proceso) {
    // Convertir el string a bytes individuales
	char* stringARetornar = malloc(tamString + 1);  // +1 para el carácter nulo
    if (stringARetornar == NULL) {
        return NULL; // Manejar error de asignación de memoria
    }
    size_t bytesRestantesPorLeer = tamString;
    size_t bytesLeidos = 0;

    size_t offsetActual = dirFisica % config_memoria.tam_pagina;
    size_t nroMarcoActual = (dirFisica - offsetActual) / config_memoria.tam_pagina;
    uint32_t siguientePagina = 0;

    // Iterar sobre las páginas del proceso para determinar la siguiente página
    for (size_t i = 0; i < list_size(proceso->listaPaginasProceso); i++) {
        t_pagina* pagina = list_get(proceso->listaPaginasProceso, i);
        if (pagina->nroMarco == nroMarcoActual) {
            siguientePagina = i + 1;
            if (siguientePagina == list_size(proceso->listaPaginasProceso)) {
                siguientePagina = 0;
            }
        }
    }

    while (bytesRestantesPorLeer > 0) {
        // Calcular la cantidad de bytes a escribir en el marco actual
        size_t espacioDisponible = config_memoria.tam_pagina - offsetActual;
        size_t bytesALeer = (bytesRestantesPorLeer < espacioDisponible) ? bytesRestantesPorLeer : espacioDisponible;

        // Escribir los bytes correspondientes en el marco actual
        memcpy(stringARetornar + bytesLeidos, memoriaPrincipal + nroMarcoActual * config_memoria.tam_pagina + offsetActual, bytesALeer);


        // Actualizar contadores
        t_pagina* pagina = list_get(proceso->listaPaginasProceso, siguientePagina);
        nroMarcoActual = pagina->nroMarco;

        siguientePagina++;
        if (siguientePagina == list_size(proceso->listaPaginasProceso)) {
            siguientePagina = 0;
        }

        bytesRestantesPorLeer -= bytesALeer;
        bytesLeidos += bytesALeer;

        // Pasar al siguiente marco y reiniciar el offset
        offsetActual = 0;  // Después del primer marco, el offset es 0
    }
	stringARetornar[tamString] = '\0'; // Asegurar que la cadena esté terminada en null
	return stringARetornar;
}

void ejecutar_copy_string(){
	
	t_buffer* buffer_recibido = recibir_buffer(fd_cpu);

    uint32_t tamString = leer_buffer_uint32(buffer_recibido);
    uint32_t direccion_fisica_si = leer_buffer_uint32(buffer_recibido);
    uint32_t direccion_fisica_di = leer_buffer_uint32(buffer_recibido);
    uint32_t pid = leer_buffer_uint32(buffer_recibido);
    destruir_buffer(buffer_recibido);
	t_proceso* proceso = buscarProcesoPorPid(pid);
	char* string_a_escribir = leerValorEnMemoriaString(direccion_fisica_si, tamString, proceso);
	
	escribirValorEnMemoriaString(string_a_escribir, direccion_fisica_di, tamString, proceso);

	enviar_codOp(fd_cpu, COPY_STRING_OK);

	// imprimir_memoria();
	free(string_a_escribir);
}

void finalizar_proceso(){
	t_buffer* buffer 		= recibir_buffer(fd_kernel);
	uint32_t pidAFinalizar  = leer_buffer_uint32(buffer);
	destruir_buffer(buffer);
	
	t_proceso* proceso_a_eliminar = buscarProcesoPorPid(pidAFinalizar);

	log_info(logger_memoria, "Destrucción: PID: %d - Tamaño: %d", proceso_a_eliminar->pid, proceso_a_eliminar->cantPaginas);

	liberarPaginasDeUnProcesoResize(proceso_a_eliminar , 0);

	// proceso_destroy(proceso_a_eliminar);

	// imprimir_memoria();
	enviar_codOp(fd_kernel, FINALIZAR_PROCESO_OK);
}

void proceso_destroy(t_proceso* proceso_a_destruir){
	list_destroy_and_destroy_elements(proceso_a_destruir->instrucciones, (void* ) destruir_instrucciones);
	free(proceso_a_destruir);

	return;
}

void destruir_instrucciones(t_instruccion* instruccion_a_destruir){
	free(instruccion_a_destruir->par1);
	free(instruccion_a_destruir->par2);
	free(instruccion_a_destruir->par3);
	free(instruccion_a_destruir->par4);
	free(instruccion_a_destruir->par5);
	free(instruccion_a_destruir);

	return;
}