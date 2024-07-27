#include "cambio_de_estado.h"

void new_a_ready(){
	while(1){
		sem_wait(&procesos_NEW);
        
        sem_wait(&grado_de_multiprogramacion);

        if(planificacion_detenida == 1){ 
            sem_wait(&pausar_new_a_ready);
        }


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
		
        sem_wait(&cpu_libre);
		// Espera a que new_a_ready se ejecute
		sem_wait(&procesos_en_ready);
		//Actualizar cuando tengamos VRR
        char* lista_pcbs_en_ready = obtener_elementos_cargados_en(colaREADY);

        log_info(logger_kernel, "Cola Ready: %s", lista_pcbs_en_ready); //OBLIGATORIO
        
        if(strcmp(config_kernel.algoritmo_planificacion,"VRR") == 0 && !queue_is_empty(colaREADYMAS)){
		    // sem_wait(&procesos_en_ready_mas);

            char* lista_pcbs_en_ready_mas = obtener_elementos_cargados_en(colaREADYMAS);
            log_info(logger_kernel, "Cola Prioridad: %s", lista_pcbs_en_ready_mas); //OBLIGATORIO
            free(lista_pcbs_en_ready_mas);
        } 
        free(lista_pcbs_en_ready);	
        
         
		if(planificacion_detenida == 1){
            sem_wait(&pausar_ready_a_exec);
        } 
		
		
		pthread_mutex_lock(&mutex_exec);
		pcb_ejecutando = retirar_pcb_de_ready_segun_algoritmo();
		pthread_mutex_unlock(&mutex_exec);


		log_info(logger_kernel, "PID: %d - Estado anterior: READY - Estado actual: EXEC", pcb_ejecutando->cde->pid); //OBLIGATORIO
        pcb_ejecutando->estado = EXEC;


		sem_post(&cont_exec); 
        enviar_cde_a_cpu(); 
        if(strcmp(config_kernel.algoritmo_planificacion, "RR") == 0 || strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0){
            pcb_ejecutando->fin_q = false;
            sem_wait(&sem_reiniciar_quantum);

            sem_post(&sem_iniciar_quantum);
            if(strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0){
                timer_vrr();
            }
        }

    }
}

void exec_a_finished(){
	while(1){
        sem_wait(&procesos_en_exit);

        t_pcb* pcb = retirar_pcb_de(colaFINISHED, &mutex_finalizados); //ok
        
        pthread_mutex_lock(&mutex_procesos_globales);
	    list_remove_element(procesos_globales, pcb);
	    pthread_mutex_unlock(&mutex_procesos_globales);
        
        //TODO
        liberar_recursos_pcb(pcb);

        // Solicitar a memoria liberar estructuras
        enviar_codOp(fd_memoria, FINALIZAR_PROCESO_SOLICITUD);

        t_buffer* buffer = crear_buffer();
        agregar_buffer_uint32(buffer, pcb->cde->pid);
        enviar_buffer(buffer, fd_memoria);
        destruir_buffer(buffer);

        mensajeKernelMem rta_memoria = recibir_codOp(fd_memoria);

        if(rta_memoria == FINALIZAR_PROCESO_OK){
            log_info(logger_kernel, "PID: %d - Destruir PCB", pcb->cde->pid);
            destruir_pcb(pcb);
        }
        else{
            log_error(logger_kernel, "Memoria no logró liberar correctamente las estructuras");
            exit(1);
        }
    }
}

void agregar_a_cola_finished(char* razon){
    sem_wait(&cont_exec);
    
    if(planificacion_detenida == 1){
        sem_wait(&pausar_exec_a_finalizado);
    }

	agregar_pcb_a(colaFINISHED, pcb_ejecutando, &mutex_finalizados);
    pcb_ejecutando->estado = EXIT;

	log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_ejecutando->cde->pid, "EXEC", "FINISHED"); //OBLIGATORIO
	log_info(logger_kernel, "Finaliza el proceso %d - Motivo: %s", pcb_ejecutando->cde->pid, razon); // OBLIGATORIO
	
    pthread_mutex_lock(&mutex_pcb_en_ejecucion);
    pcb_ejecutando = NULL;
    pthread_mutex_unlock(&mutex_pcb_en_ejecucion);

    sem_post(&cpu_libre); // se libera el procesador
	sem_post(&procesos_en_exit); // se agrega uno a procesosExit
    sem_post(&grado_de_multiprogramacion); // Se libera 1 grado de multiprog

}
void enviar_de_exec_a_ready(){
    sem_wait(&cont_exec);
    if(planificacion_detenida == 1){
        sem_wait(&pausar_exec_a_ready);
    }

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
    if(planificacion_detenida == 1){
        sem_wait(&pausar_exec_a_blocked);
    }
    
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

    if(planificacion_detenida == 1){
       sem_wait(&pausar_blocked_a_ready);
    }

    int posicion_pcb = esta_proceso_en_cola_bloqueados(pcb);

    t_pcb* pcb_a_ready = list_get(colaBLOCKED->elements,posicion_pcb);
    
    pthread_mutex_lock(&mutex_block);
    list_remove_element(colaBLOCKED->elements, pcb);
    pthread_mutex_unlock(&mutex_block);

    // t_pcb* pcb_a_ready = retirar_pcb_de(colaBLOCKED, &mutex_block);
    
    agregar_pcb_a(colaREADY, pcb_a_ready, &mutex_ready);
    pcb_a_ready->estado = READY;
    //destruir_pcb(pcb);

    
    //Agrego el pcb_a_ready a ejecutando
    // pthread_mutex_lock(&mutex_pcb_en_ejecucion);
    // pcb_ejecutando = pcb_a_ready;
    // pthread_mutex_unlock(&mutex_pcb_en_ejecucion);
    
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_a_ready->cde->pid, "BLOCKED", "READY");

    sem_post(&procesos_en_ready);

}

void enviar_pcb_de_block_a_ready_mas(t_pcb* pcb){
    sem_wait(&procesos_en_blocked);
    
    if(planificacion_detenida == 1){
       sem_wait(&pausar_blocked_a_ready);
    }

    int posicion_pcb = esta_proceso_en_cola_bloqueados(pcb);

    t_pcb* pcb_a_ready = list_get(colaBLOCKED->elements,posicion_pcb);
    
    pthread_mutex_lock(&mutex_block);
    list_remove_element(colaBLOCKED->elements, pcb);
    pthread_mutex_unlock(&mutex_block);

    // t_pcb* pcb_a_ready = retirar_pcb_de(colaBLOCKED, &mutex_block);
    
    agregar_pcb_a(colaREADYMAS, pcb_a_ready, &mutex_ready);
    pcb_a_ready->estado = READY;
    //destruir_pcb(pcb);
    //Agrego el pcb_a_ready a ejecutando
    // pthread_mutex_lock(&mutex_pcb_en_ejecucion);
    // pcb_ejecutando = pcb_a_ready;
    // pthread_mutex_unlock(&mutex_pcb_en_ejecucion);
    
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_a_ready->cde->pid, "BLOCKED", "READY+");
    // sem_post(&procesos_en_ready_mas);

    sem_post(&procesos_en_ready);

}

void enviar_de_exec_a_ready_mas(){
    sem_wait(&cont_exec);
    
    if(planificacion_detenida == 1){
        sem_wait(&pausar_exec_a_ready);
    }
    agregar_pcb_a(colaREADYMAS, pcb_ejecutando, &mutex_ready);
    pcb_ejecutando->estado = READY;
    
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_ejecutando->cde->pid, "EXEC", "READY+");

    pthread_mutex_lock(&mutex_pcb_en_ejecucion);
    pcb_ejecutando = NULL;
    pthread_mutex_unlock(&mutex_pcb_en_ejecucion);
    
    sem_post(&cpu_libre); // se libera el procesador
    sem_post(&procesos_en_ready);
}