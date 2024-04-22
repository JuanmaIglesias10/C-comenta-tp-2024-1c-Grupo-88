#include <../include/cpu.h>

t_log* logger_cpu;
t_config* config_cpu;
int fd_memoria;
int fd_cpuDispatch;
int fd_cpuInterrupt;


int main(void) {

	inicializarCpu();

	/*					  CONEXION CON MEMORIA 						*/
	fd_memoria = conectarse(config_cpu, "IP", "PUERTO_MEMORIA", "CPU", logger_cpu);
	log_info(logger_cpu,"Conexion con MEMORIA exitosa");

	/*				    INICIO EL SERVIDOR DISPATCH		 			*/
	fd_cpuDispatch = iniciar_servidor("8006", logger_cpu);

	/*				    INICIO EL SERVIDOR INTERRUPT		 		*/
	fd_cpuInterrupt = iniciar_servidor("8007", logger_cpu);

	/*				  DISPATCH ESPERA AL CLIENTE KERNEL			 	*/
	int fd_kernel_dis = esperar_cliente(fd_cpuDispatch, logger_cpu,"KERNEL(DISPATCH)"); 

	/*				  INTERRUPT ESPERA AL CLIENTE KERNEL		 	*/
	int fd_kernel_int = esperar_cliente(fd_cpuInterrupt, logger_cpu,"KERNEL(INTERRUPT)"); 
	


	pthread_t hilo_kernel_dis;
	pthread_create(&hilo_kernel_dis, NULL, (void*)atender_kernel_dis(fd_kernel_dis), NULL);
	pthread_detach(hilo_kernel_dis);

	pthread_t hilo_kernel_int;
	pthread_create(&hilo_kernel_int, NULL, (void*)atender_kernel_int(fd_kernel_int), NULL); //servidor
	pthread_detach(hilo_kernel_int);

	pthread_t hilo_cpu_memoria;
	pthread_create(&hilo_cpu_memoria, NULL, (void*)atender_memoria(fd_memoria), NULL);
	pthread_detach(hilo_cpu_memoria);

	liberar_conexion(fd_memoria);

	log_destroy(logger_cpu);
}

void inicializarCpu() {

	config_cpu = iniciar_config("../cpu.config");
	logger_cpu = iniciar_logger("logCPU.log","CPU",LOG_LEVEL_INFO);
}

