#include "kernel.h"

int main(void)
{
	inicializar_kernel(); 
	iniciar_consola();
}

void inicializar_kernel(){

	pid_a_asignar = 0;
    planificacion_detenida = 0;

	logger_kernel = iniciar_logger("logKernel.log","KERNEL",LOG_LEVEL_INFO);
	iniciar_config_kernel();
	inicializar_conexiones();
	inicializar_listas_colas();
	inicializar_semaforos();

    enviar_codOp(fd_cpu_dis, ALGORITMO_PLANIFICACION);
    t_buffer* buffer = crear_buffer();
    // si el algoritmo es FIFO envia 0
    // si el algoritmo es ROUND ROBIN envia un 1
    // si el algoritmo es VIRTUAL ROUND ROBIN envia un 2
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

    if(es_RR_o_VRR()){
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
	config_kernel.grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");

    char **recursos = config_get_array_value(config,"RECURSOS");
	char **instancias = config_get_array_value(config,"INSTANCIAS_RECURSOS");
	config_kernel.recursos = list_create();

	int i = 0;
	while(recursos[i]!= NULL){
		int num_instancias = strtol(instancias[i],NULL,10);
		t_recurso* recurso = inicializar_recurso(recursos[i], num_instancias);
		list_add(config_kernel.recursos, recurso);
		i++;
	}

    string_array_destroy(recursos);
    string_array_destroy(instancias);
}

void inicializar_conexiones(){
	fd_memoria = conectarse(config_kernel.ip_memoria , config_kernel.puerto_memoria ,"MEMORIA", logger_kernel);
	fd_cpu_dis = conectarse(config_kernel.ip_cpu ,config_kernel.puerto_cpu_dispatch, "CPU DISPATCH", logger_kernel);
	fd_cpu_int = conectarse(config_kernel.ip_cpu,config_kernel.puerto_cpu_interrupt,"CPU INTERRUPT", logger_kernel);

	fd_kernel = iniciar_servidor(config_kernel.puerto_escucha, logger_kernel);

    pthread_t hilo_IO_accept;
    pthread_create(&hilo_IO_accept, NULL, (void*)aceptar_conexiones_IO, NULL);
    pthread_detach(hilo_IO_accept);

	pthread_t hilo_cpu_dis;
	pthread_create(&hilo_cpu_dis, NULL, (void*)recibir_cde_de_cpu, NULL);
	pthread_detach(hilo_cpu_dis);

}

void inicializar_listas_colas(){
    procesos_globales = list_create();
	
    // tablaArchivosAbiertos = list_create();
    // listaArchivosGlobales = list_create();

	colaNEW = queue_create();
	colaREADY = queue_create();
    colaREADYMAS = queue_create();
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
    pthread_mutex_init(&mutex_colaGEN,NULL);
	sem_init(&cpu_libre, 0, 1);
	sem_init(&procesos_en_ready, 0, 0);
	sem_init(&procesos_NEW, 0, 0);
	sem_init(&aviso_exec, 0, 0); //Inicia en 0, posteado por ready_a_exec
	sem_init(&bin_recibir_cde, 0, 0); //Inicia en 0, posteado por ready_a_exec
	sem_init(&procesos_en_exit, 0, 0); //Inicia en 0, posteado por ready_a_exec
    sem_init(&sem_iniciar_quantum, 0, 0);
    sem_init(&sem_reiniciar_quantum, 0, 1);
    sem_init(&cont_exec, 0, 0);
    sem_init(&sem_timer, 0, 0);
    sem_init(&grado_de_multiprogramacion, 0, config_kernel.grado_multiprogramacion);
    sem_init(&pausar_new_a_ready, 0, 0);
    sem_init(&pausar_ready_a_exec, 0, 0);
    sem_init(&pausar_exec_a_finalizado, 0, 0);
    sem_init(&pausar_exec_a_ready, 0, 0);
    sem_init(&pausar_exec_a_blocked, 0, 0);
    sem_init(&pausar_blocked_a_ready, 0, 0);
    sem_init(&sem_colaGEN, 0, 1);
}

t_recurso* inicializar_recurso(char* nombre_recurso, int instancias_totales){
    t_recurso* recurso = malloc(sizeof(t_recurso));
    
    int tam = strlen(nombre_recurso);

    recurso->nombre = malloc(tam + 1); // +1 para el carácter nulo
    strcpy(recurso->nombre, nombre_recurso);

    recurso->instancias = instancias_totales;
    recurso->procesos_bloqueados = list_create();
    sem_t sem;
    sem_init(&sem, instancias_totales, 1);
    recurso->sem_recurso = sem;

    return recurso;
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

void finalizar_proceso(char* pid_string){
    // Antes de hacer finalizar proceso, hay que ver que como desde todos los estados podes pasar a exit, quiza conviene 
    // tener el estado en el pcb, para saber en que lista lo sacas? 
    // Aunque sin este parametro se puede hacer quiza es un poco mas rebuscado?

    int resultado = 0;
    uint32_t pid = atoi(pid_string);
    retirar_pcb_de_su_respectivo_estado(pid, &resultado);    
    return;
}

t_pcb* encontrar_pcb_por_pid(uint32_t pid, int* encontrado){
    t_pcb* pcb;
    
    *(encontrado) = 0;

    for(int i = 0; i < list_size(procesos_globales); i++){
        pcb = list_get(procesos_globales, i);
        if(pcb->cde->pid == pid){
            *(encontrado) = 1;
            break;
        }
    }

    if(*(encontrado))
        return pcb;
    else {
        log_warning(logger_kernel, "PCB no encontrado de PID: %d", pid);
        return NULL;
    }
}

void retirar_pcb_de_su_respectivo_estado(uint32_t pid, int* resultado){
    t_pcb* pcb_a_retirar = encontrar_pcb_por_pid(pid, resultado);
    if(pcb_a_retirar == NULL){
        log_warning(logger_kernel , "PCB encontrado se encuentra en NULL");
    }
    if(resultado){
        switch(pcb_a_retirar->estado){
            case NEW:
                sem_wait(&procesos_NEW);
                pthread_mutex_lock(&mutex_new);
                list_remove_element(colaNEW->elements, pcb_a_retirar);
                pthread_mutex_unlock(&mutex_new);
                enviar_a_finalizado(pcb_a_retirar, "EXIT POR CONSOLA");
                break;
            case READY:
                sem_wait(&procesos_en_ready);
                pthread_mutex_lock(&mutex_ready);
                list_remove_element(colaREADY->elements, pcb_a_retirar); //PARA VRR hacemos el chequeo si hay algo en ready+ o ya fue?
                pthread_mutex_unlock(&mutex_ready);
                enviar_a_finalizado(pcb_a_retirar, "EXIT POR CONSOLA");
                break;
            case BLOCKED:
                sem_wait(&procesos_en_blocked);
                pthread_mutex_lock(&mutex_block);
                list_remove_element(colaBLOCKED->elements, pcb_a_retirar);
                pthread_mutex_unlock(&mutex_block);
                enviar_a_finalizado(pcb_a_retirar, "EXIT POR CONSOLA");
                break;
            case EXEC:
                enviar_codOp(fd_cpu_int,INTERRUPT);
                
                t_buffer* buffer = crear_buffer();
                agregar_buffer_uint32(buffer, pcb_a_retirar->cde->pid);
                enviar_buffer(buffer, fd_cpu_int);
                destruir_buffer(buffer);
                break;
            case FINISHED:
                // ?? hace falta hacer algo?? dsps ver
                break;
            default:
                log_error(logger_kernel, "Entre al default en estado nro: %d", pcb_a_retirar->estado);
                break;
        
        }
    }
    else
        log_warning(logger_kernel, "No ejecute el switch");
}

void enviar_a_finalizado(t_pcb* pcb_a_finalizar, char* razon){
    agregar_pcb_a(colaFINISHED, pcb_a_finalizar, &mutex_finalizados);
    log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_a_finalizar->cde->pid, obtener_nombre_estado(pcb_a_finalizar->estado), obtener_nombre_estado(FINISHED)); //OBLIGATORIO
    log_info(logger_kernel, "Finaliza el proceso %d - Motivo: %s", pcb_a_finalizar->cde->pid, razon); // OBLIGATORIO
    sem_post(&procesos_en_exit);
    if (pcb_a_finalizar->estado == READY || pcb_a_finalizar->estado == BLOCKED)
        sem_post(&grado_de_multiprogramacion); //Como se envia a EXIT, se "libera" 1 grado de multiprog
}

void cambiar_grado_multiprogramacion(char* nuevo_grado){
    //para cambiarlo la planificacion debe estar detendida
    int grado_a_asignar = atoi(nuevo_grado);

    if(planificacion_detenida == 1){ // se puede cambiar el grado de multiprogramacion
        
        grado_de_multiprogramacion.__align = grado_a_asignar - config_kernel.grado_multiprogramacion + grado_de_multiprogramacion.__align - 1;
        sem_post(&grado_de_multiprogramacion);
        config_kernel.grado_multiprogramacion = grado_a_asignar; 
    }
    else{
        // no se puede realizar el cambio
        log_warning(logger_kernel, "La planificacion no se detuvo. No se puede cambiar el grado de multiprogramacion");
    }
}

void detenerPlanificacion(){ 
    planificacion_detenida = 1;
}

void iniciarPlanificacion(){
    sem_post(&pausar_new_a_ready);
    // if(pausar_new_a_ready.__align == 1)
    //     sem_wait(&pausar_new_a_ready);
    
    sem_post(&pausar_ready_a_exec);
    // if(pausar_ready_a_exec.__align == 1)
    //     sem_wait(&pausar_ready_a_exec);
    
    sem_post(&pausar_exec_a_finalizado);
    // if(pausar_exec_a_finalizado.__align == 1)
    //     sem_wait(&pausar_exec_a_finalizado);
    
    sem_post(&pausar_exec_a_ready);
    // if(pausar_exec_a_ready.__align == 1)
    //     sem_wait(&pausar_exec_a_ready);
    
    sem_post(&pausar_exec_a_blocked);
    // if(pausar_exec_a_blocked.__align == 1)
    //     sem_wait(&pausar_exec_a_blocked);
    
    sem_post(&pausar_blocked_a_ready);
    // if(pausar_blocked_a_ready.__align == 1)
    //     sem_wait(&pausar_blocked_a_ready);
    
    planificacion_detenida = 0;
}

void procesosPorEstado(){
    log_info(logger_kernel, "---------LISTANDO PROCESOS POR ESTADO---------");

    char* procesos_cargados_en_new = obtener_elementos_cargados_en(colaNEW);
    char* procesos_cargados_en_ready = obtener_elementos_cargados_en(colaREADY);
    char* procesos_cargados_en_blocked = obtener_elementos_cargados_en(colaBLOCKED);
    char* procesos_cargados_en_exit = obtener_elementos_cargados_en(colaFINISHED);

    log_info(logger_kernel, "Procesos en NEW: %s", procesos_cargados_en_new);
    log_info(logger_kernel, "Procesos en READY: %s", procesos_cargados_en_ready);
    
     if(pcb_ejecutando != NULL)
        log_info(logger_kernel, "Proceso en EXEC: [%d]", pcb_ejecutando->cde->pid);
    else
        log_info(logger_kernel, "Proceso en EXEC: []");
    
    log_info(logger_kernel, "Procesos en BLOCKED: %s", procesos_cargados_en_blocked);
    log_info(logger_kernel, "Procesos en FINISHED: %s", procesos_cargados_en_exit);

    free(procesos_cargados_en_new);
    free(procesos_cargados_en_ready);
    free(procesos_cargados_en_blocked);
    free(procesos_cargados_en_exit);
}

t_pcb* crear_PCB(){

	t_pcb* PCB_creado = malloc(sizeof(t_pcb));
	PCB_creado->cde = malloc(sizeof(t_cde));
	PCB_creado->cde->registros = malloc(sizeof(t_registros));
    PCB_creado->quantum = config_kernel.quantum ;

	PCB_creado->cde->pid = pid_a_asignar; 
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
    PCB_creado->recursos_asignados = list_create();
    PCB_creado->recursos_solicitados = list_create();

	pid_a_asignar++; //Aumento en 1 el PID
    PCB_creado->flag_clock = false;
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
    // free(parametros);
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

void destruir_pcb(t_pcb* pcb){
    destruir_cde(pcb->cde);
    list_destroy(pcb->recursos_asignados);
    list_destroy(pcb->recursos_solicitados);
    free(pcb);
}

char* obtener_elementos_cargados_en(t_queue* cola){
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
    }else if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0){
        if(!queue_is_empty(colaREADYMAS)){
            t_pcb* pcb = retirar_pcb_de(colaREADYMAS, &mutex_ready);
            return pcb;
        } else {
            t_pcb* pcb = retirar_pcb_de(colaREADY, &mutex_ready);
            return pcb;
        } 
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

void enviar_cde_a_cpu() {
	enviar_codOp(fd_cpu_dis, EJECUTAR_PROCESO);

    t_buffer* buffer = crear_buffer();
    pthread_mutex_lock(&mutex_pcb_en_ejecucion); 
	agregar_buffer_cde(buffer, pcb_ejecutando->cde);
    pthread_mutex_unlock(&mutex_pcb_en_ejecucion);

     if(es_RR_o_VRR() ){
         pcb_ejecutando->flag_clock = false;
     }

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
        if (pcb_ejecutando->cde == NULL) log_error(logger_kernel, "CDE NULL");
        pthread_mutex_unlock(&mutex_exec);
        t_instruccion* instruccion_actual = leer_buffer_instruccion(buffer);
        
        if(strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0){
            temporal_stop(timer);
            sem_post(&sem_timer);
        }
        evaluar_instruccion(instruccion_actual);
        
        
        destruir_buffer(buffer);

    }
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
        pthread_mutex_lock(&mutex_pcb_en_ejecucion);
        uint32_t pid_pcb_before_start_clock = pcb_ejecutando->cde->pid;
        bool flag_clock_pcb_before_start_clock = pcb_ejecutando->flag_clock; //aranca en false
        
        usleep(pcb_ejecutando->quantum * 1000);

        if(pcb_ejecutando != NULL) pcb_ejecutando->fin_q = true;
        pthread_mutex_unlock(&mutex_pcb_en_ejecucion);

        if(pcb_ejecutando != NULL && pid_pcb_before_start_clock == pcb_ejecutando->cde->pid && flag_clock_pcb_before_start_clock == pcb_ejecutando->flag_clock){
           
            enviar_codOp(fd_cpu_int, DESALOJO);
            t_buffer* buffer = crear_buffer();
            agregar_buffer_uint32(buffer, pcb_ejecutando->cde->pid); // lo enviamos porque interrupt recibe un buffer, pero no hacemos nada con esto
            enviar_buffer(buffer, fd_cpu_int);
            destruir_buffer(buffer);
        }
        sem_post(&sem_reiniciar_quantum);

    }

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

void timer_vrr(){
    timer = temporal_create(); 
    sem_wait(&sem_timer);
    ms_transcurridos = temporal_gettime(timer); 
    temporal_destroy(timer);
}

// EVALUAR INSTRUCCIONES

void evaluar_instruccion(t_instruccion* instruccion_actual){
    switch(instruccion_actual->codigo){
        case WAIT:
            if(es_RR_o_VRR()){
                pcb_ejecutando->flag_clock = true;
            } // si se bloquea tengo que reiniciar el clock para el nuevo proceso (RR-VRR)
            char* nombre_recurso = instruccion_actual->par1;
            evaluar_wait(nombre_recurso);
            destruir_instruccion(instruccion_actual);
            break;
        case SIGNAL:
            if(es_RR_o_VRR()){
                pcb_ejecutando->flag_clock = true;
            }
            char* nombre_recurso_signal = instruccion_actual->par1;
            evaluar_signal(nombre_recurso_signal);
            destruir_instruccion(instruccion_actual);
            break;      
        case IO_GEN_SLEEP:
            if(es_RR_o_VRR()){
                pcb_ejecutando->flag_clock = true;
            }   
            io_gen_sleep(instruccion_actual->par1, instruccion_actual->par2);
            destruir_instruccion(instruccion_actual);
            break;
        case IO_STDIN_READ:
            if(es_RR_o_VRR()){
                pcb_ejecutando->flag_clock = true;
            }
            io_stdin_read(instruccion_actual->par1,instruccion_actual->par2,instruccion_actual->par3);
            destruir_instruccion(instruccion_actual);
            break;
        case IO_STDOUT_WRITE:
            if(es_RR_o_VRR()){
                pcb_ejecutando->flag_clock = true;
            }
            io_stdout_write(instruccion_actual->par1,instruccion_actual->par2,instruccion_actual->par3);
            destruir_instruccion(instruccion_actual);
            break;
        case OUT_OF_MEMORY_VUELTA:
            if(es_RR_o_VRR()){
                pcb_ejecutando->flag_clock = true;
            }
            log_warning(logger_kernel, "PID: %d - OUT OF MEMORY", pcb_ejecutando->cde->pid);
            agregar_a_cola_finished("OUT OF MEMORY");
            destruir_instruccion(instruccion_actual);
            break;
        case EXIT:
            if(es_RR_o_VRR()){
                pcb_ejecutando->flag_clock = true;
            }
            agregar_a_cola_finished("SUCCESS");
            destruir_instruccion(instruccion_actual);
            break;
        case EXIT_POR_CONSOLA:
            if(es_RR_o_VRR()){
                pcb_ejecutando->flag_clock = true;
            }
            agregar_a_cola_finished("EXIT POR CONSOLA");
            destruir_instruccion(instruccion_actual);
            break;
        default: // FIN DE QUANTUM
        log_info(logger_kernel, "PID: %d - Desalojado por fin de Quantum", pcb_ejecutando->cde->pid);
        if (strcmp(config_kernel.algoritmo_planificacion,"VRR") == 0) {
            pcb_ejecutando->quantum = config_kernel.quantum; 
        }
        enviar_de_exec_a_ready();
        destruir_instruccion(instruccion_actual);

        break;
        
    }
}

void io_gen_sleep(char* interfaz, char* char_unidadesDeTrabajo) {
    uint8_t unidadesDeTrabajo = atoi(char_unidadesDeTrabajo);


    pthread_mutex_lock(&mutex_colaGEN);
    t_interfaz* aux = queue_pop(colaGenerica);
    pthread_mutex_unlock(&mutex_colaGEN);
    

    if (strcmp(aux->nombre, interfaz) == 0 ){
        if (strcmp(aux->tipo , "GENERICA") == 0) {
        enviar_codOp(aux->fd,SLEEP);
        t_buffer* buffer_a_enviar = crear_buffer();
        agregar_buffer_uint8(buffer_a_enviar,unidadesDeTrabajo);
        enviar_buffer(buffer_a_enviar,aux->fd);
        destruir_buffer(buffer_a_enviar);
        
        //Bloqueo el proceso
        t_pcb* pcb_sleep = malloc(sizeof(t_pcb)); 
        pcb_sleep = pcb_ejecutando;
                     
        enviar_de_exec_a_block();
        
        mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if(codigo == SLEEP_OK) {
                if(strcmp(config_kernel.algoritmo_planificacion,"VRR") == 0 && ms_transcurridos < pcb_sleep->quantum){
                    pcb_sleep->quantum -= ms_transcurridos;
                    queue_push(colaGenerica,aux);
                    sem_post(&sem_colaGEN);
                    enviar_pcb_de_block_a_ready_mas(pcb_sleep);
                    
                } else {
                    queue_push(colaGenerica,aux);
                    sem_post(&sem_colaGEN);
                    enviar_pcb_de_block_a_ready(pcb_sleep);
                }
            }
        
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    }
    else if(strcmp(aux->nombre, interfaz) != 0){
        agregar_a_cola_finished("INVALID_INTERFACE");
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");

    }
    //free(interfaz);
}

void evaluar_wait(char* nombre_recurso_pedido){
    int coincidencia = 0;
    int posicion_recurso;
    
    for(int i=0; i < list_size(config_kernel.recursos); i++){
        t_recurso* recurso = list_get(config_kernel.recursos, i);
        char* nombre_recurso = recurso->nombre;
        if(string_equals_ignore_case(nombre_recurso_pedido, nombre_recurso)){
            coincidencia++;
            posicion_recurso = i;
        }
    }
    if(coincidencia>0){ // Significa que el recurso existe
        t_recurso* recurso = list_get(config_kernel.recursos, posicion_recurso);
        recurso->instancias--;
        
        if(recurso->instancias < 0){  // Chequea si debe bloquear al proceso por falta de instancias

            sem_t semaforo_recurso = recurso->sem_recurso;
        	sem_wait(&semaforo_recurso);

        	list_add(recurso->procesos_bloqueados, pcb_ejecutando);
        	recurso->instancias = 0;
        	sem_post(&semaforo_recurso);

            log_info(logger_kernel, "PID: %d - Bloqueado por: %s", pcb_ejecutando->cde->pid, nombre_recurso_pedido);

            list_add(pcb_ejecutando->recursos_solicitados, recurso);
            
            enviar_de_exec_a_block();
        }
        else{
            list_add(pcb_ejecutando->recursos_asignados, recurso);

            if(es_RR_o_VRR() && !pcb_ejecutando->fin_q)
                enviar_cde_a_cpu();
            else if(es_RR_o_VRR() && pcb_ejecutando->fin_q)
                enviar_de_exec_a_ready();
            else
                enviar_cde_a_cpu();
        }
    }
    else{ // el recurso no existe
        agregar_a_cola_finished("INVALID_RESOURCE"); //TODO: codigo de inexistencia de recurso
    }
}

void evaluar_signal(char* nombre_recurso_pedido) {
    int coincidencia = 0;
    int posicion_recurso = -1; // Inicializado a un valor inválido por defecto
    int asignado = 0;

    // Primer bucle: Buscar el recurso en config_kernel.recursos
    for (int i = 0; i < list_size(config_kernel.recursos); i++) {
        t_recurso* recurso = list_get(config_kernel.recursos, i);
        char* nombre_recurso = recurso->nombre;
        if (string_equals_ignore_case(nombre_recurso_pedido, nombre_recurso)) {
            coincidencia++;
            posicion_recurso = i;
        }
    }

    // Segundo bucle: Verificar si el recurso está asignado a pcb_ejecutando
    for (int i = 0; i < list_size(pcb_ejecutando->recursos_asignados); i++) {
        t_recurso* recurso = list_get(pcb_ejecutando->recursos_asignados, i);
        char* nombre_recurso = recurso->nombre;
        if (string_equals_ignore_case(nombre_recurso_pedido, nombre_recurso)) {
            asignado++;
        }
    }

    if (coincidencia > 0 && asignado > 0) { // el recurso existe y lo tiene asignado
        t_recurso* recurso = list_get(config_kernel.recursos, posicion_recurso);
        recurso->instancias++; // Incrementar instancias del recurso

        list_remove_element(pcb_ejecutando->recursos_asignados, recurso); // Remover el recurso de los asignados

        // Enviar señales dependiendo del algoritmo y estado del quantum
        if (es_RR_o_VRR() && !pcb_ejecutando->fin_q) {
            enviar_cde_a_cpu();
        } else if (es_RR_o_VRR() && pcb_ejecutando->fin_q) {
            enviar_de_exec_a_ready();
        } else {
            enviar_cde_a_cpu();
        }

        // Desbloquear el primer proceso de la cola de bloqueados si existe
        if (list_size(recurso->procesos_bloqueados) > 0) {
            t_pcb* pcb = list_remove(recurso->procesos_bloqueados, 0); // Remover el primer proceso bloqueado

            list_remove_element(pcb->recursos_solicitados, recurso); // Remover el recurso solicitado

            list_add(pcb->recursos_asignados, recurso); // Asignar el recurso al proceso desbloqueado
            recurso->instancias--; // Decrementar instancias del recurso
            enviar_pcb_de_block_a_ready(pcb); // Mover el proceso de bloqueado a ready
        }
    } else if (coincidencia > 0 && asignado == 0) { // el recurso existe y NO lo tiene asignado
        exec_a_finished("INVALID_RESOURCE");
    } else if (coincidencia == 0 && asignado == 0) { // el recurso NO existe y NO lo tiene asignado
        exec_a_finished("INVALID_RESOURCE");
    }
}

bool es_RR_o_VRR() {
    return (strcmp(config_kernel.algoritmo_planificacion,"RR")== 0 || strcmp(config_kernel.algoritmo_planificacion,"VRR")== 0);
}

void io_stdin_read(char* interfaz, char* char_direccion_fisica, char* char_tamanio) {
    uint32_t direccion_fisica = atoi(char_direccion_fisica);
    uint32_t tamanio = atoi(char_tamanio);

    t_interfaz* aux = queue_pop(colaSTDIN);

    if (strcmp(aux->nombre, interfaz) == 0) {
        if (strcmp(aux->tipo, "STDIN") == 0) {
            enviar_codOp(aux->fd, STDIN_READ);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, direccion_fisica);
            agregar_buffer_uint32(buffer_a_enviar, tamanio);
            agregar_buffer_uint32(buffer_a_enviar, pcb_ejecutando->cde->pid);

            enviar_buffer(buffer_a_enviar, aux->fd);
            destruir_buffer(buffer_a_enviar);

            // Bloqueo el proceso
            t_pcb* pcb_read_stdin = pcb_ejecutando;
            enviar_de_exec_a_block();

            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == STDIN_READ_OK) {
                log_info(logger_kernel, "Llegue a STDIN_READ_OK");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_read_stdin->quantum) {
                    pcb_read_stdin->quantum -= ms_transcurridos;
                    enviar_pcb_de_block_a_ready_mas(pcb_read_stdin);
                } else {
                    enviar_pcb_de_block_a_ready(pcb_read_stdin);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }
    queue_push(colaSTDIN, aux);
}



void io_stdout_write(char* interfaz, char* char_direccion_fisica, char* char_tamanio) {
    uint32_t direccion_fisica   = atoi(char_direccion_fisica);
    uint32_t tamanio             = atoi(char_tamanio);

    t_interfaz* aux = queue_pop(colaSTDOUT);

    if (strcmp(aux->nombre, interfaz) == 0 ){
        if (strcmp(aux->tipo , "STDOUT") == 0) {
            enviar_codOp(aux->fd, STDOUT_WRITE);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, direccion_fisica);
            agregar_buffer_uint32(buffer_a_enviar, tamanio);
            agregar_buffer_uint32(buffer_a_enviar, pcb_ejecutando->cde->pid);

            enviar_buffer(buffer_a_enviar, aux->fd);

            destruir_buffer(buffer_a_enviar);

            // Bloqueo el proceso
            t_pcb* pcb_read_stdin = pcb_ejecutando;
            enviar_de_exec_a_block();

            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == STDOUT_WRITE_OK) {
                log_info(logger_kernel, "Llegue a STDOUT_WRITE_OK");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_read_stdin->quantum) {
                    pcb_read_stdin->quantum -= ms_transcurridos;
                    enviar_pcb_de_block_a_ready_mas(pcb_read_stdin);
                } else {
                    enviar_pcb_de_block_a_ready(pcb_read_stdin);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }
    queue_push(colaSTDOUT, aux);
}


    // TODO: falta un else


void liberar_recursos_pcb(t_pcb* pcb){
    t_recurso* recurso;
    for(int i = 0; i < list_size(pcb->recursos_asignados); i++){
        recurso = list_get(pcb->recursos_asignados, i);
        signal_recursos_asignados_pcb(pcb, recurso->nombre);
    }
    
    while(list_size(pcb->recursos_asignados) != 0){
        list_remove(pcb->recursos_asignados, 0);
    }

    while(list_size(pcb->recursos_solicitados) != 0){ // Aca entraria solo en el caso de que se finalice el proceso por consola
        recurso = list_remove(pcb->recursos_solicitados, 0);
        list_remove_element(recurso->procesos_bloqueados, pcb);
    }
}

void signal_recursos_asignados_pcb(t_pcb* pcb, char* nombre_recurso_pedido){
    int posicion_recurso;
    for(int i=0; i < list_size(config_kernel.recursos); i++){
        t_recurso* recurso = list_get(config_kernel.recursos, i);
        char* nombre_recurso = recurso->nombre;
        if(strcmp(nombre_recurso_pedido, nombre_recurso) == 0){
            posicion_recurso = i;
            break;
        }
    }
    t_recurso* recurso = list_get(config_kernel.recursos, posicion_recurso);
    recurso->instancias++; //podria considerarse chequear que no se pase de las instancias totales del recurso, pero me parecio innecesario
    log_info(logger_kernel, "PID: %d - LIBERANDO INSTANCIAS DEL RECURSO: %s - INSTANCIAS DISPONIBLES: %d", pcb->cde->pid, nombre_recurso_pedido, recurso->instancias);

    if(list_size(recurso->procesos_bloqueados) > 0){ // Desbloquea al primer proceso de la cola de bloqueados del recurso
	    sem_t semaforo_recurso = recurso->sem_recurso;

	    sem_wait(&semaforo_recurso);

		t_pcb* pcb_a_retirar = list_remove(recurso->procesos_bloqueados, 0);
		
		sem_post(&semaforo_recurso);
        
        // le asigno al pcb que se "libero" el recurso asi puede ejecutar
        recurso->instancias--;
        list_add(pcb_a_retirar->recursos_asignados, recurso);
        
        enviar_pcb_de_block_a_ready(pcb_a_retirar);
	}
}