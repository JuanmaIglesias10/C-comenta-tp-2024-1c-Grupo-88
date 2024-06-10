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

    enviar_codOp(fd_cpu_dis, ALGORITMO_PLANIFICACION);
    t_buffer* buffer = crear_buffer();
    // si el algoritmo es FIFO envia 0
    // si el algoritmo es ROUND ROBIN envia un 1
    // si el algoritmo es VIRTUAL ROUND ROBBIN envia un 2
    if(strcmp(config_kernel.algoritmo_planificacion, "FIFO") == 0) {
        agregar_buffer_uint32(buffer, 0);
        }
    else if(strcmp(config_kernel.algoritmo_planificacion, "RR") == 0){
        agregar_buffer_uint32(buffer, 1);
    }
    else{
        agregar_buffer_uint32(buffer, 2);
        }
    enviar_buffer(buffer, fd_cpu_dis);
    destruir_buffer(buffer);

    if(strcmp(config_kernel.algoritmo_planificacion, "RR") == 0 || strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0){
        prender_quantum();
    }

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

    pthread_t hilo_IO_accept;
    pthread_create(&hilo_IO_accept, NULL, (void*)aceptar_conexiones_IO, (void*)&fd_kernel);
    pthread_detach(hilo_IO_accept);
    
	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_memoria, NULL);
	pthread_detach(hilo_memoria);

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

    colaGenerica = queue_create();
    colaSTDIN = queue_create();
    colaSTDOUT = queue_create();
    colaDIALFS = queue_create();
}

void inicializar_semaforos(){
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_procesos_globales, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	pthread_mutex_init(&mutex_pcb_en_ejecucion, NULL);
	pthread_mutex_init(&mutex_finalizados, NULL);
    pthread_mutex_init(&mutex_colasIO,NULL);
    pthread_mutex_init(&mutex_block,NULL);
	sem_init(&cpu_libre, 0, 1);
	sem_init(&procesos_en_ready, 0, 0);
	sem_init(&procesos_NEW, 0, 0);
	sem_init(&aviso_exec, 0, 0); //Inicia en 0, posteado por ready_a_exec
	sem_init(&bin_recibir_cde, 0, 0); //Inicia en 0, posteado por ready_a_exec
	sem_init(&procesos_en_exit, 0, 0); //Inicia en 0, posteado por ready_a_exec
    sem_init(&sem_iniciar_quantum, 0, 0);
    sem_init(&sem_reiniciar_quantum, 0, 1);
    sem_init(&cont_exec, 0, 0);
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
    PCB_creado->fin_q = false;
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

		sem_post(&cont_exec); 
        
        if(strcmp(config_kernel.algoritmo_planificacion, "RR") == 0){
            pcb_ejecutando->fin_q = false;
            sem_wait(&sem_reiniciar_quantum);
            sem_post(&sem_iniciar_quantum);
        }
		
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

    sem_post(&bin_recibir_cde); 
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

void io_gen_sleep() {
	mensajeKernelCpu codOp = recibir_codOp(fd_cpu_int);

	if (codOp == INTERRUPT) {
		// Recibir buffer y extraer lo recibido
		t_buffer* buffer_recibido = recibir_buffer(fd_cpu_int);
		uint8_t unidadesDeTiempo  = leer_buffer_uint8(buffer_recibido);
		char* interfaz = leer_buffer_string(buffer_recibido); //Int1 
		destruir_buffer(buffer_recibido);
        
        // Chequeo de si existe la interfaz y coincide el tipo
        t_interfaz* aux = queue_pop(colaGenerica);
        
        if (strcmp(aux->nombre, interfaz) == 0 ){
            if (strcmp(aux->tipo , "GENERICA") == 0) {
		    //Mandarlo a IO GENERICA y BLOQUEAR PROCESO
		    enviar_codOp(fd_IO,SLEEP);
		    t_buffer* buffer_a_enviar = crear_buffer();
		    agregar_buffer_uint8(buffer_a_enviar,unidadesDeTiempo);
		    enviar_buffer(buffer_a_enviar,aux->fd);
		    destruir_buffer(buffer_a_enviar);
            
            //Bloqueo el proceso
            t_pcb* pcb_sleep = malloc(sizeof(t_pcb)); // REVISAR ESTO, NECESITO COPIAR LA INFO DEL PCB EJECUTANDO EN OTRO AUXILIAR.
            pcb_sleep = pcb_ejecutando;             
            enviar_de_exec_a_block();

            //Espero el OK de la interfaz para volver a ponerlo en ready
            mensajeKernelIO codigo = recibir_codOp(aux->fd);
                if(codigo == SLEEP_OK) {
                    enviar_pcb_de_block_a_ready(pcb_sleep);
                }
            
            } else {
                agregar_a_cola_finished("Interfaz no coincide el tipo");
            }
        }
        else if(strcmp(aux->nombre, interfaz) != 0){
            agregar_a_cola_finished("No coincide el nombre de la interfaz");
        } else {
            agregar_a_cola_finished("No existe la interfaz");

        }
    }
	// ¿¿Algo mas??
}



void evaluar_instruccion(t_instruccion* instruccion_actual){
    switch(instruccion_actual->codigo){
        case IO_GEN_SLEEP:
            io_gen_sleep();
            break;
        case EXIT:
            // if(strcmp(config_kernel.algoritmo, "RR") == 0){
            //     pcb_en_ejecucion->flag_clock = true;
            // }
            agregar_a_cola_finished("SUCCESS");
            destruir_instruccion(instruccion_actual);
            break;
        default: // es por fin de quantum
            if(strcmp(config_kernel.algoritmo_planificacion, "RR") == 0)
                log_info(logger_kernel, "PID: %d - Desalojado por fin de Quantum", pcb_ejecutando->cde->pid);
            else if(strcmp(config_kernel.algoritmo_planificacion, "PRIORIDADES") == 0)
                log_info(logger_kernel, "PID: %d - Desalojado por un proceso de mayor prioridad", pcb_ejecutando->cde->pid);
            enviar_de_exec_a_ready();
            destruir_instruccion(instruccion_actual);
            break;
    }
}


void agregar_a_cola_finished(char* razon){
    
    sem_wait(&cont_exec);
    
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

// Round Robin

void prender_quantum(){
    pthread_t reloj_rr;
    pthread_create(&reloj_rr, NULL, (void*) controlar_tiempo_de_ejecucion, NULL);
    pthread_detach(reloj_rr);
}

void controlar_tiempo_de_ejecucion(){
    while(1){
        sem_wait(&sem_iniciar_quantum);
        // uint32_t pid_pcb_before_start_clock = pcb_en_ejecucion->cde->pid;
        // bool flag_clock_pcb_before_start_clock = pcb_en_ejecucion->flag_clock; //aranca en false

        usleep(config_kernel.quantum * 1000);

        // if(pcb_en_ejecucion != NULL)
        //     pcb_en_ejecucion->fin_q = true;

        // if(pcb_en_ejecucion != NULL && pid_pcb_before_start_clock == pcb_en_ejecucion->cde->pid && flag_clock_pcb_before_start_clock == pcb_en_ejecucion->flag_clock){
        enviar_codOp(fd_cpu_int, DESALOJO);

        t_buffer* buffer = crear_buffer();
        agregar_buffer_uint32(buffer, pcb_ejecutando->cde->pid); // lo enviamos porque interrupt recibe un buffer, pero no hacemos nada con esto
        enviar_buffer(buffer, fd_cpu_int);
        destruir_buffer(buffer);
        // }
        sem_post(&sem_reiniciar_quantum);
    }

}

void enviar_de_exec_a_ready(){
    sem_wait(&cont_exec);
    // if(planificacion_detenida == 1){
    //     sem_wait(&pausar_exec_a_ready);
    // }
    agregar_pcb_a(colaREADY, pcb_ejecutando, &mutex_ready);
    pcb_ejecutando->estado = READY;
    
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_ejecutando->cde->pid, "EXEC", "READY");

    pthread_mutex_lock(&mutex_pcb_en_ejecucion);
    pcb_ejecutando = NULL;
    pthread_mutex_unlock(&mutex_pcb_en_ejecucion);
    
    sem_post(&cpu_libre); // se libera el procesador
    sem_post(&procesos_en_ready);
}

void enviar_de_exec_a_block(){
    sem_wait(&cont_exec);
    //if(planificacion_detenida == 1){
    //    sem_wait(&pausar_exec_a_blocked);
    //}
    agregar_pcb_a(colaBLOCKED, pcb_ejecutando, &mutex_block);
    pcb_ejecutando->estado = BLOCKED;

    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_ejecutando->cde->pid, "EXEC", "BLOCKED");
    
    pthread_mutex_lock(&mutex_pcb_en_ejecucion);
    pcb_ejecutando = NULL;
    pthread_mutex_unlock(&mutex_pcb_en_ejecucion);
    

    sem_post(&cpu_libre); // se libera el procesador
    sem_post(&procesos_en_blocked);
}

void enviar_pcb_de_block_a_ready(t_pcb* pcb){
    
    sem_wait(&procesos_en_blocked);
    
   // if(planificacion_detenida == 1){
   //     sem_wait(&pausar_blocked_a_ready);
   // }

    int posicion_pcb = esta_proceso_en_cola_bloqueados(pcb);

    t_pcb* pcb_a_ready = list_get(colaBLOCKED->elements,posicion_pcb);
    
    pthread_mutex_lock(&mutex_block);
    list_remove_element(colaBLOCKED->elements, pcb);
    pthread_mutex_unlock(&mutex_block);

    // t_pcb* pcb_a_ready = retirar_pcb_de(colaBLOCKED, &mutex_block);
    
    agregar_pcb_a(colaREADY, pcb_a_ready, &mutex_ready);
    pcb_a_ready->estado = READY;
    
    //Agrego el pcb_a_ready a ejecutando
    // pthread_mutex_lock(&mutex_pcb_en_ejecucion);
    // pcb_ejecutando = pcb_a_ready;
    // pthread_mutex_unlock(&mutex_pcb_en_ejecucion);
    
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_a_ready->cde->pid, "BLOCKED", "READY");

    sem_post(&procesos_en_ready);
}

int esta_proceso_en_cola_bloqueados(t_pcb* pcb){
    int posicion_pcb = -1;
    for(int i=0; i < list_size(colaBLOCKED->elements); i++){
        t_pcb* pcb_get = list_get(colaBLOCKED->elements, i);
        int pid_pcb = pcb_get->cde->pid;
        if(pcb->cde->pid == pid_pcb){
            posicion_pcb = i;
            break;
        }
    }
    return posicion_pcb;
}