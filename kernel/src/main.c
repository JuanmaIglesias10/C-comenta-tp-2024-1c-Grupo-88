#include <../include/main.h>

int main(void)
{
	t_log * logger;

	t_config *config =iniciar_config();
	logger = iniciar_logger("logKernel.log","Client",LOG_LEVEL_INFO);

	conectarse(config,"IP","PUERTO_CPU","Kernel");
}
