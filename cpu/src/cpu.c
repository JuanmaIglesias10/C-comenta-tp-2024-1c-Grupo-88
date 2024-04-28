#include "cpu.h"

int main(void) {

	inicializarCpu();

}

void inicializarCpu() {

	logger_cpu = iniciar_logger("logCPU.log","CPU",LOG_LEVEL_INFO);
	inicializar_config();
	inicializar_conexiones();

}

void inicializar_config(){
	config = iniciar_config("./cpu.config");
	config_cpu.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	config_cpu.puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
	config_cpu.puerto_escucha_dispatch = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
	config_cpu.puerto_escucha_interrupt = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");
	config_cpu.cantidad_entradas_tlb = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
	config_cpu.algoritmo_tlb = config_get_string_value(config, "ALGORITMO_TLB");
}

void inicializar_conexiones(){
	fd_memoria = conectarse(config_cpu.ip_memoria, config_cpu.puerto_memoria, "MEMORIA", logger_cpu);

	fd_cpu_dis = iniciar_servidor(config_cpu.puerto_escucha_dispatch, logger_cpu);

	fd_cpu_int = iniciar_servidor(config_cpu.puerto_escucha_interrupt, logger_cpu);

	fd_kernel_dis = esperar_cliente(fd_cpu_dis, logger_cpu,"KERNEL(DISPATCH)"); 

	fd_kernel_int = esperar_cliente(fd_cpu_int, logger_cpu,"KERNEL(INTERRUPT)"); 
	


	pthread_t hilo_kernel_dis;
	pthread_create(&hilo_kernel_dis, NULL, (void*)atender_kernel_dis(fd_kernel_dis), NULL);
	pthread_detach(hilo_kernel_dis);

	pthread_t hilo_kernel_int;
	pthread_create(&hilo_kernel_int, NULL, (void*)atender_kernel_int(fd_kernel_int), NULL);
	pthread_detach(hilo_kernel_int);

	pthread_t hilo_cpu_memoria;
	pthread_create(&hilo_cpu_memoria, NULL, (void*)atender_memoria(fd_memoria), NULL);
	pthread_detach(hilo_cpu_memoria);

	liberar_conexion(fd_memoria);

	log_destroy(logger_cpu);
}