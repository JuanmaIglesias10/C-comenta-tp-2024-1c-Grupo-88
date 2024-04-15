#include <../include/main.h>

int main(void) {


	// ------------  ENTRADA/SALIDA ------> KERNEL -------------------
	
	t_config* config = iniciar_config("../entradaSalida.config");
	t_log* logger = iniciar_logger("logEntradaSalida.log","cliente",LOG_LEVEL_INFO);

	conectarse(config, "IP", "PUERTO_KERNEL", "entradaSalida", logger);

	
}

