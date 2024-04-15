#include <../include/main.h>

int main(void)
{
	// ----------- KERNEL ----> CPU ------------------------

	t_config *config = iniciar_config("../kernel.config");
	t_log * logger = iniciar_logger("logKernel.log","Client",LOG_LEVEL_INFO);

	conectarse(config,"IP","PUERTO_CPU","Kernel", logger);

	// ----------- ENTRADA_SALIDA ----> KERNEL ------------------------

	logger = iniciar_logger("logKernel.log", "Server", LOG_LEVEL_DEBUG);
	server_escuchar(logger, "4446");
}