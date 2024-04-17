#include <../include/main.h>

int main(void)
{
	t_config *config = iniciar_config("../kernel.config");
	t_log * logger = iniciar_logger("logKernel.log","Client",LOG_LEVEL_INFO);


	/*					CONEXION CON CPU DISPATCH					*/
	conectarse(config,"IP","PUERTO_CPU_DISPATCH","KERNEL", logger);
	log_info(logger,"Conexion con CPU_DISPATCH exitosa");

	/*					CONEXION CON CPU INTERRUPT					*/
	conectarse(config,"IP","PUERTO_CPU_INTERRUPT","KERNEL", logger);
	log_info(logger,"Conexion con CPU_INTERRUPT exitosa");

	/*					  CONEXION CON MEMORIA						*/
	conectarse(config, "IP", "PUERTO_MEMORIA", "KERNEL", logger);
	log_info(logger,"Conexion con MEMORIA exitosa");


	log_destroy(logger);

}