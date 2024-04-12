#include <../include/main.h>

int main(void)
{
	int conexion;
	char *ip;
	char *puerto;

	
	t_config *config =iniciar_config();

	t_log* logger = iniciar_logger("logCliente.log","Cliente de cpu",LOG_LEVEL_INFO);

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	conexion = crear_conexion(ip, puerto);

	enviar_mensaje("hola, soy el kernel", conexion);
	liberar_conexion(conexion);
	config_destroy(config);
}
