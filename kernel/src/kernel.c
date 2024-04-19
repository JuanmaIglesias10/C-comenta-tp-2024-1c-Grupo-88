#include <../include/kernel.h>

int main(void)
{
	t_config *config_kernel = iniciar_config("../kernel.config");
	t_log * logger_kernel = iniciar_logger("logKernel.log","Client",LOG_LEVEL_INFO);

	/*					  CONEXION CON MEMORIA						*/
	int fd_memoria = conectarse(config_kernel, "IP", "PUERTO_MEMORIA", "KERNEL", logger_kernel);
	log_info(logger_kernel,"Conexion con MEMORIA exitosa");

	/*					CONEXION CON CPU DISPATCH					*/
	int fd_cpu_dis = conectarse(config_kernel,"IP","PUERTO_CPU_DISPATCH","KERNEL", logger_kernel);
	log_info(logger_kernel,"Conexion con CPU DISPATCH exitosa");

	/*					CONEXION CON CPU INTERRUPT					*/
	int fd_cpu_int = conectarse(config_kernel,"IP","PUERTO_CPU_INTERRUPT","KERNEL", logger_kernel);
	log_info(logger_kernel,"Conexion con CPU INTERRUPT exitosa");

	/*				    INICIO EL SERVIDOR KERNEL			 		*/
	int fd_kernel = iniciar_servidor("4446", logger_kernel);

	/*				     KERNEL ESPERA AL CLIENTE IO			 	*/
	int fd_IO = esperar_cliente(fd_kernel, logger_kernel,"IO"); 

	pthread_t hilo_cpu_dis;
	pthread_create(&hilo_cpu_dis, NULL, (void*)atender_cpu_dis(logger_kernel,fd_cpu_dis), NULL);
	pthread_detach(hilo_cpu_dis);

	pthread_t hilo_cpu_int;
	pthread_create(&hilo_cpu_int, NULL, (void*)atender_cpu_int(logger_kernel,fd_cpu_int), NULL);
	pthread_detach(hilo_cpu_int);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_memoria(logger_kernel,fd_memoria), NULL);
	pthread_detach(hilo_memoria);

	pthread_t hilo_IO;
	pthread_create(&hilo_IO, NULL, (void*)atender_IO(logger_kernel,fd_IO), NULL);
	pthread_detach(hilo_IO);

	liberar_conexion(fd_memoria);
	liberar_conexion(fd_cpu_dis);
	liberar_conexion(fd_cpu_int);


}