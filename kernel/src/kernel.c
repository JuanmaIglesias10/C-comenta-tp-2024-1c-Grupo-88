#include "kernel.h"

int main(void)
{
	inicializar_kernel(); 
	inciar_consola();

}

void inicializar_kernel(){
	logger_kernel = iniciar_logger("logKernel.log","KERNEL",LOG_LEVEL_INFO);
	iniciar_config_kernel();
	inicializar_conexiones();
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

	// liberar_conexion(fd_memoria);
	// liberar_conexion(fd_cpu_dis);
	// liberar_conexion(fd_cpu_int);
}

void inciar_consola(){
	char* input;
	while(1){
		mostrar_opciones_consola();
		input  = readline(">");
		if (strcmp(input, "EXIT") == 0) {
            free(input);
            break;
		}
		// add_history(input);
		atender_consola(input);
		free(input);
	}
}

void mostrar_opciones_consola(){
	printf("---MENU---\n");
	printf("1. EJECUTAR_SCRIPT\n"); //PATH => Archivo de comandos, se encuentra en maquina donde corra el kernel 1
	printf("2. INICIAR_PROCESO\n"); //PATH => Archivo pseudocodigo memoria 1
	printf("3. FINALIZAR_PROCESO\n"); //PID 1
	printf("4. INICIAR_PLANIFICACION\n"); //0
	printf("5. DETENER_PLANIFICACION\n"); //0
	printf("6. MULTIPROGRAMACION\n"); //VALOR 1
	printf("7. PROCESO_ESTADO\n"); // 0
	printf("8. EXIT\n"); //0
}


void atender_consola(char* input){ 
	char** lista_mensaje = string_split(input, " ");
	int cant_par = string_array_size(lista_mensaje) - 1;
	t_codigo_operacion cod_operacion = get_codigo_operacion(lista_mensaje[0], cant_par);

	switch(cod_operacion){
		case EJECUTAR_SCRIPT:
			// ejecutar_script(lista_mensaje[1]);
			string_array_destroy(lista_mensaje); 
            break;
    	case INICIAR_PROCESO:
			// iniciarProceso(lista_mensaje[1]);
			string_array_destroy(lista_mensaje); 
            break;
    	case FINALIZAR_PROCESO:
			// finalizarProceso(lista_mensaje[1]);
			//detectarDeadlock();
			string_array_destroy(lista_mensaje); 
    		break;
    	case INICIAR_PLANIFICACION:
    		log_info(logger_kernel, "INICIO DE PLANIFICACIÓN");
			// iniciarPlanificacion();
			string_array_destroy(lista_mensaje);
    		break;
    	case DETENER_PLANIFICACION:
    		log_info(logger_kernel, "PAUSA DE PLANIFICACIÓN");
			// detenerPlanificacion();
			string_array_destroy(lista_mensaje);
    		break;
    	case MULTIPROGRAMACION:
			// cambiar_grado_multiprogramacion(lista_mensaje[1]);
			string_array_destroy(lista_mensaje);
    		break;
    	case PROCESO_ESTADO:
			// procesosPorEstado();
			string_array_destroy(lista_mensaje); 
    		break;
		case E_PARAMETROS:
			log_info(logger_kernel, "Error en los parametros pasados");
			string_array_destroy(lista_mensaje);
			break;
		case MENSAJE_VACIO:
			log_info(logger_kernel, "Mensaje recibido vacio");
			string_array_destroy(lista_mensaje);
			break;
		case BASURA:
			log_info(logger_kernel, "Operacion desconocida");
			string_array_destroy(lista_mensaje);
			break;
		default:
			log_warning(logger_kernel,"Operacion desconocida (default)");
			string_array_destroy(lista_mensaje);
			break;
    }
}

t_codigo_operacion get_codigo_operacion(char* comando, int cant_par){
	if(strcmp(comando, "EJECUTAR_SCRIPT") == 0){
		if(cant_par != 1){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: EJECUTAR_SCRIPT");
		return EJECUTAR_SCRIPT;
	}
	if(strcmp(comando, "INICIAR_PROCESO") == 0){
		if(cant_par != 1){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: INICIAR_PROCESO");
		return INICIAR_PROCESO;
	}
	else if(strcmp(comando, "FINALIZAR_PROCESO") == 0){
		if(cant_par != 1){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: FINALIZAR_PROCESO");
		return FINALIZAR_PROCESO;
	}
	else if(strcmp(comando, "INICIAR_PLANIFICACION") == 0){
		if(cant_par != 0){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: INICIAR_PLANIFICACION");
		return INICIAR_PLANIFICACION;
	}
	else if(strcmp(comando, "DETENER_PLANIFICACION") == 0){
		if(cant_par != 0){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: DETENER_PLANIFICACION");
		return DETENER_PLANIFICACION;
	}
	else if(strcmp(comando, "MULTIPROGRAMACION") == 0){
		if(cant_par != 1){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: MULTIPROGRAMACION");
		return MULTIPROGRAMACION;
	}
	else if(strcmp(comando, "PROCESO_ESTADO") == 0){
		if(cant_par != 0){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: PROCESO_ESTADO");
		return PROCESO_ESTADO;
	}
	else if(strcmp(comando, "") == 0){
		log_info(logger_kernel, "No se recibio ningun valor.");
		return MENSAJE_VACIO;
	}
	else{
		log_info(logger_kernel, "El valor ingresado no es correcto.");
		return BASURA;
	}
}

   
t_pcb* creando_PCB(char* path, int prioridad){

	t_pcb* PCB_creado = malloc(sizeof(t_pcb));
	PCB_creado->registros = malloc(sizeof(registros_cpu));

	PCB_creado->pid = pid_a_asignar;
	PCB_creado->prioridad = prioridad;
	PCB_creado->registros->ax=0;
	PCB_creado->registros->bx=0;
	PCB_creado->registros->cx=0;
	PCB_creado->registros->dx=0;
	PCB_creado->registros->eax=0;
	PCB_creado->registros->ebx=0;
	PCB_creado->registros->ecx=0;
	PCB_creado->registros->edx=0;
	PCB_creado->registros->si=0;
	PCB_creado->registros->di=0;

	PCB_creado->path = path;
	PCB_creado->estado= NULO;
	// PCB_creado->program_counter= 0;
	// PCB_creado->quantum=0;
	return 0;
}