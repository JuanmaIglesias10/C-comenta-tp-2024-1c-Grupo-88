#include "kernel.h"

int main(void)
{
	inicializar_kernel(); 
	inciar_consola();
}

void inicializar_kernel(){

	pid_a_asignar = 0;

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

void iniciarProceso(char* path) {
	t_pcb* pcb_nuevo = crear_PCB(path);
	enviar_codOp(fd_memoria , INICIAR_PROCESO_SOLICITUD);
	
	t_buffer* bufferKernel = crear_buffer();

	agregar_buffer_uint32(bufferKernel,pcb_nuevo->pid);
	agregar_buffer_string(bufferKernel,pcb_nuevo->path)
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
