#include <../include/config.h>


t_config* iniciar_config(void)
{
	t_config *nuevo_config = config_create("kernel.config");
	if (nuevo_config == NULL)
	{
		perror("Hubo un error en las CONFIG");
		exit(EXIT_FAILURE);
	}
	return nuevo_config;
}