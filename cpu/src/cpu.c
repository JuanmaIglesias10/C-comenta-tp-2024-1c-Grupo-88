#include <../include/cpu.h>

int main(void) {


	t_config* config = iniciar_config("../cpu.config");
	t_log *logger_cpu = iniciar_logger("logCPU.log","CPU",LOG_LEVEL_INFO);

	/*					  CONEXION CON MEMORIA 						*/
	int fd_memoria = conectarse(config, "IP", "PUERTO_MEMORIA", "CPU", logger_cpu);
	log_info(logger_cpu,"Conexion con MEMORIA exitosa");

	/*				    INICIO EL SERVIDOR DISPATCH		 			*/
	int fd_cpuDispatch = iniciar_servidor("8006", logger_cpu);

	/*				    INICIO EL SERVIDOR INTERRUPT		 		*/
	int fd_cpuInterrupt = iniciar_servidor("8007", logger_cpu);

	/*				  DISPATCH ESPERA AL CLIENTE KERNEL			 	*/
	int fd_kernel_dis = esperar_cliente(fd_cpuDispatch, logger_cpu,"KERNEL(DISPATCH)"); 

	/*				  INTERRUPT ESPERA AL CLIENTE KERNEL		 	*/
	int fd_kernel_int = esperar_cliente(fd_cpuInterrupt, logger_cpu,"KERNEL(INTERRUPT)"); 
	


	pthread_t hilo_kernel_dis;
	pthread_create(&hilo_kernel_dis, NULL, (void*)atender_kernel_dis(logger_cpu,fd_kernel_dis), NULL);
	pthread_detach(hilo_kernel_dis);

	pthread_t hilo_kernel_int;
	pthread_create(&hilo_kernel_int, NULL, (void*)atender_kernel_int(logger_cpu,fd_kernel_int), NULL);
	pthread_detach(hilo_kernel_int);

	pthread_t hilo_cpu_memoria;
	pthread_create(&hilo_cpu_memoria, NULL, (void*)atender_memoria(logger_cpu, fd_memoria), NULL);
	pthread_detach(hilo_cpu_memoria);

	liberar_conexion(fd_memoria);

	log_destroy(logger_cpu);
}

