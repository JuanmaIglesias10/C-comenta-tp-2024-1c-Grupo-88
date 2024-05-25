#include "kernel.h"

int main(void)
{
	inicializar_kernel(); 
	iniciar_consola();
}

void inicializar_kernel(){

	pid_a_asignar = 0;

	logger_kernel = iniciar_logger("logKernel.log","KERNEL",LOG_LEVEL_INFO);
	iniciar_config_kernel();
	inicializar_conexiones();
	inicializar_listas_colas();
	inicializar_semaforos();
	inicializar_planificadores();

	

}

void iniciar_config_kernel(){
	config = config_create("./kernel.config");
	config_kernel.puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	config_kernel.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	config_kernel.puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
	config_kernel.ip_cpu = config_get_string_value(config, "IP_CPU");
	config_kernel.puerto_cpu_dispatch = config_get_int_value(config, "PUERTO_CPU_DISPATCH");
	config_kernel.puerto_cpu_interrupt = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
	config_kernel.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	config_kernel.quantum = config_get_int_value(config, "QUANTUM");
	// config_kernel.recursos = config_get_string_value(config, "RECURSOS");
	// config_kernel.instancias_recursos = config_get_string_value(config, "INSTANCIAS_RECURSOS");
	config_kernel.grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
}

void inicializar_conexiones(){
	fd_memoria = conectarse(config_kernel.ip_memoria , config_kernel.puerto_memoria ,"MEMORIA", logger_kernel);
	fd_cpu_dis = conectarse(config_kernel.ip_cpu ,config_kernel.puerto_cpu_dispatch, "CPU DISPATCH", logger_kernel);
	fd_cpu_int = conectarse(config_kernel.ip_cpu,config_kernel.puerto_cpu_interrupt,"CPU INTERRUPT", logger_kernel);

	fd_kernel = iniciar_servidor(config_kernel.puerto_escucha, logger_kernel);

	fd_IO = esperar_cliente(fd_kernel, logger_kernel,"IO"); 

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_memoria, NULL);
	pthread_detach(hilo_memoria);

	pthread_t hilo_IO;
	pthread_create(&hilo_IO, NULL, (void*)atender_IO, NULL);
	pthread_detach(hilo_IO);

	pthread_t hilo_cpu_dis;
	pthread_create(&hilo_cpu_dis, NULL, (void*)recibir_cde_de_cpu, NULL);
	pthread_detach(hilo_cpu_dis);

	pthread_t hilo_cpu_int;
	pthread_create(&hilo_cpu_int, NULL, (void*)atender_cpu_int, NULL);
	pthread_detach(hilo_cpu_int);

}

void inicializar_listas_colas(){
    procesos_globales = list_create();
	// procesosBloqueadosPageFault = list_create();
    // tablaArchivosAbiertos = list_create();
    // listaArchivosGlobales = list_create();

	colaNEW = queue_create();
	colaREADY = queue_create();
	colaBLOCKED = queue_create();
	colaFINISHED = queue_create();
}

void inicializar_semaforos(){
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_procesos_globales, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	pthread_mutex_init(&mutex_pcb_en_ejecucion, NULL);
	pthread_mutex_init(&mutex_finalizados, NULL);
	sem_init(&cpu_libre, 0, 1);
	sem_init(&procesos_en_ready, 0, 0);
	sem_init(&procesos_NEW, 0, 0);
	sem_init(&aviso_exec, 0, 0); //Inicia en 0, posteado por ready_a_exec
	sem_init(&bin_recibir_cde, 0, 0); //Inicia en 0, posteado por ready_a_exec
	sem_init(&procesos_en_exit, 0, 0); //Inicia en 0, posteado por ready_a_exec

}

t_list* ejecutar_script(char* pathScript){
	t_list* scriptProcesos = obtener_lista_script(pathScript);
	return scriptProcesos;
	
}

void iniciar_proceso(char* path) {
	t_pcb* pcb_nuevo = crear_PCB();
	enviar_codOp(fd_memoria , INICIAR_PROCESO_SOLICITUD);
	
	t_buffer* bufferKernel = crear_buffer();
	//Agrego el path del pid y el path del .txt

	agregar_buffer_uint32(bufferKernel,pcb_nuevo->cde->pid);
	agregar_buffer_string(bufferKernel, path);
	//Envio el buffer a memoria
	enviar_buffer(bufferKernel, fd_memoria);
	destruir_buffer(bufferKernel);

    mensajeKernelMem cod_op = recibir_codOp(fd_memoria);

	if(cod_op == INICIAR_PROCESO_OK){
        // Poner en new
        agregar_pcb_a(colaNEW, pcb_nuevo, &mutex_new);

        pthread_mutex_lock(&mutex_procesos_globales);
        list_add(procesos_globales, pcb_nuevo); // agrego a lista global de pcbs
        pthread_mutex_unlock(&mutex_procesos_globales);

        pcb_nuevo->estado = NEW;

        log_info(logger_kernel, "Se crea el proceso %d en NEW", pcb_nuevo->cde->pid);
        sem_post(&procesos_NEW);
    }
    else if(cod_op == INICIAR_PROCESO_ERROR)
        log_info(logger_kernel, "No se pudo crear el proceso %d", pcb_nuevo->cde->pid);
    
}

t_pcb* crear_PCB(){

	t_pcb* PCB_creado = malloc(sizeof(t_pcb));
	PCB_creado->cde = malloc(sizeof(t_cde));
	PCB_creado->cde->registros = malloc(sizeof(t_registros));


	// PCB_creado->path = path; //Path del .txt
	PCB_creado->cde->pid = pid_a_asignar; 
	// PCB_creado->quantum=0;
	PCB_creado->cde->registros->PC= 0;
	PCB_creado->cde->registros->AX=0;
	PCB_creado->cde->registros->BX=0;
	PCB_creado->cde->registros->CX=0;
	PCB_creado->cde->registros->DX=0;
	PCB_creado->cde->registros->EAX=0;
	PCB_creado->cde->registros->EBX=0;
	PCB_creado->cde->registros->ECX=0;
	PCB_creado->cde->registros->EDX=0;
	PCB_creado->cde->registros->SI=0;
	PCB_creado->cde->registros->DI=0;
	PCB_creado->estado = NULO;

	pid_a_asignar++; //Aumento en 1 el PID

	return PCB_creado;
}

t_list* obtener_lista_script(char* pathScript){
	t_list* listaScript = list_create();
	FILE* archivo = fopen(pathScript ,"r");
	char linea[100];
	char* parametros;
	t_script* script;
	while (fgets(linea, sizeof(linea), archivo)) {
		//INICIAR_PROCESO Hola.txt
		strtok(linea, "\n");
		script = malloc(sizeof(t_script));
        script->par1 = NULL;
        parametros = strtok(linea, " ");
        script->codigo_operacion = obtener_codigo_operacion(parametros);
		parametros = strtok(NULL, " ");
		if(parametros != NULL){
			script->par1 = malloc(strlen(parametros) + 1);
			strcpy(script->par1, parametros);
			string_append(&(script->par1), "\0");
		}
		list_add(listaScript,script);
	}
	fclose(archivo);
	return listaScript;

}

t_codigo_operacion obtener_codigo_operacion(char* parametro) {

	if(strcmp(parametro,"EJECUTAR_SCRIPT") == 0)
		return EJECUTAR_SCRIPT;
	if(strcmp(parametro,"INICIAR_PROCESO") == 0)
		return INICIAR_PROCESO;
	if(strcmp(parametro,"FINALIZAR_PROCESO") == 0)
		return FINALIZAR_PROCESO;
	if(strcmp(parametro,"INICIAR_PLANIFICACION") == 0)
		return INICIAR_PLANIFICACION;
	if(strcmp(parametro,"DETENER_PLANIFICACION") == 0)
		return DETENER_PLANIFICACION;
	if(strcmp(parametro,"MULTIPROGRAMACION") == 0)
		return MULTIPROGRAMACION;
	if(strcmp(parametro,"PROCESO_ESTADO") == 0)
		return PROCESO_ESTADO;
	return EXIT_FAILURE;
}

void inicializar_planificadores(){
	inicializar_largo_plazo();
	inicializar_corto_plazo();
}

void inicializar_largo_plazo(){
	pthread_t hiloNew;
	pthread_t hiloFinished;

	pthread_create(&hiloNew, NULL, (void*)new_a_ready, NULL);
	pthread_detach(hiloNew);
	
	pthread_create(&hiloFinished, NULL, (void*)exec_a_finished, NULL);
	pthread_detach(hiloFinished);
} 

void inicializar_corto_plazo(){
	pthread_t planificadorCortoPlazo;
	pthread_create(&planificadorCortoPlazo, NULL,(void*)ready_a_exec, NULL);
	pthread_detach(planificadorCortoPlazo);
}

void new_a_ready(){
	while(1){
		sem_wait(&procesos_NEW);
        
        // sem_wait(&grado_de_multiprogramacion);

        // if(planificacion_detenida == 1){ 
        //     sem_wait(&pausar_new_a_ready);
        // }


        t_pcb* pcb_a_ready = retirar_pcb_de(colaNEW, &mutex_new);
        agregar_pcb_a(colaREADY, pcb_a_ready, &mutex_ready);
        pcb_a_ready->estado = READY;

        // Pedir a memoria incializar estructuras

        log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_a_ready->cde->pid, "NEW", "READY"); //OBLIGATORIO
        
        
        sem_post(&procesos_en_ready);
	}
}

void ready_a_exec(){
	while(1){
		// 
        sem_wait(&cpu_libre);
		// Espera a que new_a_ready se ejecute
		sem_wait(&procesos_en_ready);
		//Actualizar cuando tengamos VRR
        char* lista_pcbs_en_ready = obtener_elementos_cargados_en(colaREADY);
        log_info(logger_kernel, "Cola Ready de algoritmo %s: %s", config_kernel.algoritmo_planificacion, lista_pcbs_en_ready); //OBLIGATORIO
        free(lista_pcbs_en_ready);	

        /* 
		if(planificacion_detenida == 1){
            sem_wait(&pausar_ready_a_exec);
        } 
		*/
		
		pthread_mutex_lock(&mutex_exec);
		pcb_ejecutando = retirar_pcb_de_ready_segun_algoritmo();
		pthread_mutex_unlock(&mutex_exec);
		

		log_info(logger_kernel, "PID: %d - Estado anterior: READY - Estado actual: EXEC", pcb_ejecutando->cde->pid); //OBLIGATORIO
        pcb_ejecutando->estado = EXEC;

		sem_post(&aviso_exec); 
        
       /*  if(strcmp(config_kernel.algoritmo, "RR") == 0){
            pcb_ejecutando->fin_q = false;
            sem_wait(&sem_reloj_destruido);
            sem_post(&sem_iniciar_quantum);
        }
		 */
        enviar_cde_a_cpu();
	}
}

void exec_a_finished(){
	while(1){
        sem_wait(&procesos_en_exit);

        t_pcb* pcb = retirar_pcb_de(colaFINISHED, &mutex_finalizados); //ok

        pthread_mutex_lock(&mutex_procesos_globales);
	    list_remove_element(procesos_globales, pcb);
	    pthread_mutex_unlock(&mutex_procesos_globales);
		log_info(logger_kernel, "soy un pijudo");
        // liberar_recursos_pcb(pcb);
        // liberar_archivos_pcb(pcb);

        // // Solicitar a memoria liberar estructuras
        // enviar_codigo(socket_memoria, FINALIZAR_PROCESO_SOLICITUD);

        // t_buffer* buffer = crear_buffer_nuestro();
        // buffer_write_uint32(buffer, pcb->cde->pid);
        // enviar_buffer(buffer, socket_memoria);
        // destruir_buffer_nuestro(buffer);

        // mensajeKernelMem rta_memoria = recibir_codigo(socket_memoria);

        // if(rta_memoria == FINALIZAR_PROCESO_OK){
        //     log_info(logger_kernel, "PID: %d - Destruir PCB", pcb->cde->pid);
        //     destruir_pcb(pcb);
        // }
        // else{
        //     log_error(logger_kernel, "Memoria no logró liberar correctamente las estructuras");
        //     exit(1);
        // }
        
    }
}


/*
Para poder pasar procesos a finished, necesito tenerlos en la cola de finished
Para tenerlos en la cola de finished, necesito agregarlos a la cola con un MOTIVO
Como tenemos FIFO, el unico motivo para tenerlos en la cola de finished es tener, en la ultima instruccion, EXIT


*/



char* obtener_elementos_cargados_en(t_queue* cola){ //Hace un string de los pid en ready, de esta manera [1,2,3]
    char* aux = string_new();
    string_append(&aux,"[");
    int pid_aux;
    char* aux_2;
    for(int i = 0 ; i < list_size(cola->elements); i++){
        t_pcb* pcb = list_get(cola->elements,i);
        pid_aux = pcb->cde->pid;
        aux_2 = string_itoa(pid_aux);
        string_append(&aux, aux_2);
        free(aux_2);
        if(i != list_size(cola->elements)-1)
            string_append(&aux,", ");
    }
    string_append(&aux,"]");
    return aux;
}

t_pcb* retirar_pcb_de_ready_segun_algoritmo(){
    if(strcmp(config_kernel.algoritmo_planificacion, "FIFO") == 0 || strcmp(config_kernel.algoritmo_planificacion, "RR") == 0){
        t_pcb* pcb = retirar_pcb_de(colaREADY, &mutex_ready);
        return pcb;
    }
    else{
        log_info(logger_kernel, "No se reconoce el algoritmo definido en el config_kernel");
    }
    exit(1); 
}

void agregar_pcb_a(t_queue* cola, t_pcb* pcb_a_agregar, pthread_mutex_t* mutex){
    
    pthread_mutex_lock(mutex);
    queue_push(cola, (void*) pcb_a_agregar);
	pthread_mutex_unlock(mutex);

}

t_pcb* retirar_pcb_de(t_queue* cola, pthread_mutex_t* mutex){
    
	pthread_mutex_lock(mutex);
	t_pcb* pcb = queue_pop(cola);
	pthread_mutex_unlock(mutex);
    
	return pcb;
}

char* obtener_nombre_estado(t_estado_proceso estado){
    switch(estado){
		case NULO:
            return "NULO";
        case NEW:
            return "NEW";
            break;
        case READY:
            return "READY";
            break;
        case EXEC:
            return "EXEC";
            break;
        case BLOCKED:
            return "BLOCKED";
            break;
        case FINISHED:
            return "FINISHED";
            break;
        default:
			return "Proceso no reconocido";
            break;
    }
}

void enviar_cde_a_cpu(){
    
	enviar_codOp(fd_cpu_dis, EJECUTAR_PROCESO);

    t_buffer* buffer = crear_buffer();
    pthread_mutex_lock(&mutex_pcb_en_ejecucion); //ESTE SEMAFORO NI IDEA DONDE MAS ESTÁ
	agregar_buffer_cde(buffer, pcb_ejecutando->cde);
	// agregar_buffer_uint32(buffer, pcb_ejecutando->cde->pid);
	// agregar_buffer_registros(buffer, pcb_ejecutando->cde->registros);
    pthread_mutex_unlock(&mutex_pcb_en_ejecucion);

    // if(strcmp(config_kernel.algoritmo, "RR") == 0){
    //     pcb_en_ejecucion->flag_clock = false;
    // }

    enviar_buffer(buffer, fd_cpu_dis);
    destruir_buffer(buffer);

    sem_post(&bin_recibir_cde); //ESTE SEMAFORO NI IDEA DONDE MAS ESTÁ
}

void recibir_cde_de_cpu(){
	while(1){
        sem_wait(&bin_recibir_cde);
        t_buffer* buffer = recibir_buffer(fd_cpu_dis);
        pthread_mutex_lock(&mutex_exec);
        destruir_cde(pcb_ejecutando->cde);        
        pcb_ejecutando->cde = leer_buffer_cde(buffer);
        pthread_mutex_unlock(&mutex_exec);

        t_instruccion* instruccion_actual = leer_buffer_instruccion(buffer);
        /*
        if(strcmp(config_kernel.algoritmo, "RR") == 0 && pcb_en_ejecucion->fin_q && (instruccion_actual->codigo == MOV_IN || instruccion_actual->codigo == MOV_OUT)){
            log_info(logger_kernel, "PID: %d - Desalojado por fin de Quantum", pcb_en_ejecucion->cde->pid);
            enviar_de_exec_a_ready();
            destruir_instruccion(instruccion_actual);
        }

        else if(instruccion_actual->codigo == MOV_IN || instruccion_actual->codigo == MOV_OUT){
            uint32_t nro_pagina = buffer_read_uint32(buffer);
            pthread_t hilo_page_fault;
            pthread_create(&hilo_page_fault, NULL, (void *) recibir_page_fault, (void *) &nro_pagina);
            pthread_detach(hilo_page_fault);
            destruir_instruccion(instruccion_actual);
        }
        else if(instruccion_actual->codigo == F_READ || instruccion_actual->codigo == F_WRITE){
            uint8_t motivo = buffer_read_uint8(buffer);
            if(motivo == HAY_PAGE_FAULT){
                uint32_t nro_pagina = buffer_read_uint32(buffer);          
                pthread_t hilo_page_fault;
                pthread_create(&hilo_page_fault, NULL, (void *) recibir_page_fault, (void *) &nro_pagina);
                pthread_detach(hilo_page_fault);
                destruir_instruccion(instruccion_actual);
                }
            else if(motivo == DIRECCION_FISICA_OK){
                uint32_t dir_fisica = buffer_read_uint32(buffer);
                uint32_t nro_pagina = buffer_read_uint32(buffer);
                instruccion_actual->par2 = string_itoa(dir_fisica);
                instruccion_actual->par3 = string_itoa(nro_pagina);
                evaluar_instruccion(instruccion_actual);
            }
        }
        else{
		}*/
        evaluar_instruccion(instruccion_actual);
        
        
        destruir_buffer(buffer);
    }
}

void recibir_dormirIO() {
	mensajeKernelCpu codOp = recibir_codOp(fd_cpu_int);

	if (codOp == INTERRUPT) {
		// Recibir buffer y extraer lo recibido
		t_buffer* buffer_recibido = recibir_buffer(fd_cpu_int);
		uint8_t unidadesDeTiempo  = leer_buffer_uint8(buffer_recibido);
		uint8_t interfaz = leer_buffer_uint8(buffer_recibido);
		destruir_buffer(buffer_recibido);

		//Mandarlo a IO GENERICA
		enviar_codOp(fd_IO,SLEEP);
		t_buffer* buffer_a_enviar = crear_buffer();
		agregar_buffer_uint8(buffer_a_enviar,unidadesDeTiempo);
		enviar_buffer(buffer_a_enviar,fd_IO);
		destruir_buffer(buffer_a_enviar);

		// ¿¿Algo mas??
	}
}


void evaluar_instruccion(t_instruccion* instruccion_actual){
    switch(instruccion_actual->codigo){    
        case EXIT:
            // if(strcmp(config_kernel.algoritmo, "RR") == 0){
            //     pcb_en_ejecucion->flag_clock = true;
            // }
            agregar_a_cola_finished("SUCCESS");
            destruir_instruccion(instruccion_actual);
            break;
        default: // es por fin de quantum
            // if(strcmp(config_kernel.algoritmo, "RR") == 0)
            //     log_info(logger_kernel, "PID: %d - Desalojado por fin de Quantum", pcb_en_ejecucion->cde->pid);
            // else if(strcmp(config_kernel.algoritmo, "PRIORIDADES") == 0)
            //     log_info(logger_kernel, "PID: %d - Desalojado por un proceso de mayor prioridad", pcb_en_ejecucion->cde->pid);
            // enviar_de_exec_a_ready();
            // destruir_instruccion(instruccion_actual);
            break;
    }
}


void agregar_a_cola_finished(char* razon){
    // sem_wait(&cont_exec);
    
    // if(planificacion_detenida == 1){
    //     sem_wait(&pausar_exec_a_finalizado);
    // }

	agregar_pcb_a(colaFINISHED, pcb_ejecutando, &mutex_finalizados);
    pcb_ejecutando->estado = EXIT;

	log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_ejecutando->cde->pid, "EXEC", "FINISHED"); //OBLIGATORIO
	log_info(logger_kernel, "Finaliza el proceso %d - Motivo: %s", pcb_ejecutando->cde->pid, razon); // OBLIGATORIO
	
    pthread_mutex_lock(&mutex_pcb_en_ejecucion);
    pcb_ejecutando = NULL;
    pthread_mutex_unlock(&mutex_pcb_en_ejecucion);

    sem_post(&cpu_libre); // se libera el procesador
	sem_post(&procesos_en_exit); // se agrega uno a procesosExit
    // sem_post(&grado_de_multiprogramacion); // Se libera 1 grado de multiprog
}