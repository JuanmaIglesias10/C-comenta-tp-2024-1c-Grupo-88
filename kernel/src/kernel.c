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
	pthread_create(&hilo_cpu_dis, NULL, (void*)atender_cpu_dis, NULL);
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


	sem_init(&procesos_NEW, 0, 0);

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
	PCB_creado->cde->program_counter= 0;
	// PCB_creado->quantum=0;
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

// void planificador_largo_plazo{
// 	pthread_t new_ready;
// 	pthread_create(&new_ready, NULL, (void*)new_a_ready, NULL);


// }

// void new_a_ready(){
// 	while(1){
// 		sem_wait(&procesos_NEW);
        
//         // sem_wait(&grado_de_multiprogramacion);

//         // if(planificacion_detenida == 1){ 
//         //     sem_wait(&pausar_new_a_ready);
//         // }

//         t_pcb* pcb_a_ready = retirar_pcb_de(colaNEW, &mutex_new);
        
//         agregar_pcb_a(colaNEW, pcb_a_ready, &mutex_ready);
//         pcb_a_ready->estado = READY;

//         // Pedir a memoria incializar estructuras

//         log_info(logger_kernel, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb_a_ready->cde->pid, obtener_nombre_estado(NEW), obtener_nombre_estado(READY)); //OBLIGATORIO
        
//         if(strcmp(config_kernel.algoritmo, "PRIORIDADES") == 0 && pcb_en_ejecucion != NULL){
//             t_pcb* posible_a_ejecutar = pcb_con_mayor_prioridad_en_ready();

//             if(pcb_en_ejecucion->prioridad > posible_a_ejecutar->prioridad){
//                 enviar_codigo(socket_cpu_interrupt, DESALOJO);

//                 t_buffer* buffer = crear_buffer_nuestro();
//                 buffer_write_uint32(buffer, posible_a_ejecutar->cde->pid); // lo enviamos porque interrupt recibe un buffer, pero no hacemos nada con esto
//                 enviar_buffer(buffer, socket_cpu_interrupt);
//                 destruir_buffer_nuestro(buffer);
//             }
//         }
        
//         sem_post(&procesos_en_ready);
// 	}
// }


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
