#include <../include/main.h>

int main(void) {


	// ------------  ENTRADA/SALIDA CLIENTE -------------------
	
	t_config* config = iniciar_config("");
	t_log* logger = iniciar_logger("logEntradaSalida.log","cliente",LOG_LEVEL_INFO);

	conectarse(config, "IP", "PUERTO_MEMORIA", "cpu", logger);
}
