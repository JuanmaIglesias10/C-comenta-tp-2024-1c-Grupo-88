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

t_list* ejecutar_script(char* pathScript){
	t_list* scriptProcesos = parsear_archivo_script(pathScript);
	return scriptProcesos;
	
}

void iniciar_proceso(char* path) {
	t_pcb* pcb_nuevo = crear_PCB(path);
	enviar_codOp(fd_memoria , INICIAR_PROCESO_SOLICITUD);
	
	t_buffer* bufferKernel = crear_buffer();
	//Agrego el path del pid y el path del .txt
	agregar_buffer_uint32(bufferKernel,pcb_nuevo->pid);
	agregar_buffer_string(bufferKernel, pcb_nuevo->path);
	//Envio el buffer a memoria
	enviar_buffer(bufferKernel, fd_memoria);
	destruir_buffer(bufferKernel);

	// Recibo codigo de MEMORIA
	mensajeKernelMem codigo_operacion =  recibir_codigo(fd_memoria);

	

	// TO DO lo demas
}

// Crear PCB
t_pcb* crear_PCB(char* path){

	t_pcb* PCB_creado = malloc(sizeof(t_pcb));
	PCB_creado->registros = malloc(sizeof(registros_cpu));

	PCB_creado->pid = pid_a_asignar;
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
	PCB_creado->estado = NULO;
	PCB_creado->program_counter= 0;
	PCB_creado->quantum=0;

	pid_a_asignar++;
	return PCB_creado;
}


t_list* parsear_archivo_script(char* pathScript){
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