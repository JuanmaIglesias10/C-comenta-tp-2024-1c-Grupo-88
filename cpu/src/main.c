#include <../include/main.h>

int main(void) {


	t_config* config = iniciar_config("../cpu.config");
	t_log *logger = iniciar_logger("logCPU.log","CPU",LOG_LEVEL_INFO);


	/*				    INICIO EL SERVIDOR DISPATCH		 			*/
	int fd_cpuDispatch = iniciar_servidor("8006", logger);

	/*				    INICIO EL SERVIDOR INTERRUPT		 		*/
	int fd_cpuInterrupt = iniciar_servidor("8007", logger);


	/*					  CONEXION CON MEMORIA 						*/
	conectarse(config, "IP", "PUERTO_MEMORIA", "CPU", logger);
	log_info(logger,"Conexion con MEMORIA exitosa");
	

	/*				  DISPATCH ESPERA AL CLIENTE KERNEL			 	*/
	int fd_cpuDispatch_kernel = esperar_cliente(fd_cpuDispatch, logger,"KERNEL"); 

	/*				  INTERRUPT ESPERA AL CLIENTE KERNEL		 	*/
	int fd_cpuInterrupt_kernel = esperar_cliente(fd_cpuInterrupt, logger,"KERNEL"); 
	


	pthread_t hilo_cpuDispatch_kernel;
	pthread_t hilo_cpuInterrupt_kernel;

	pthread_create(&hilo_cpuDispatch_kernel, NULL, (void*)server_escuchar(logger,fd_cpuDispatch_kernel,"CPU"), NULL);
	pthread_detach(hilo_cpuDispatch_kernel);

	pthread_create(&hilo_cpuInterrupt_kernel, NULL, (void*)server_escuchar(logger,fd_cpuInterrupt_kernel,"KERNEL"), NULL);
	pthread_detach(hilo_cpuInterrupt_kernel);


	log_destroy(logger);
}

