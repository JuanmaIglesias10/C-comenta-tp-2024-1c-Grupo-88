#include <stdlib.h>
#include <stdio.h>
#include <../include/main.h>

int main(void)
{
	int conexion;
	char *ip;
	char *puerto;

	
	t_config *config =iniciar_config();
	t_log* logger = iniciar_logger();

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	conexion = crear_conexion(ip, puerto);

	enviar_mensaje("hola, soy el kernel", conexion);
	liberar_conexion(conexion);
	config_destroy(config);
}

t_config *iniciar_config(void)
{
	t_config *nuevo_config = config_create("kernel.config");
	if (nuevo_config == NULL)
	{
		perror("Hubo un error en las CONFIG");
		exit(EXIT_FAILURE);
	}
	return nuevo_config;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "nombre_proceso", true, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL) {
		printf("no se pudo crear el log");
		exit(3);
	}
	return nuevo_logger;
}