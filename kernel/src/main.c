#include <../include/main.h>

int main(void)
{
	

	t_config *config = iniciar_config("../kernel.config");
	t_log * logger = iniciar_logger("logKernel.log","Client",LOG_LEVEL_INFO);

	conectarse(config,"IP","PUERTO_CPU","Kernel", logger);
}