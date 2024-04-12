#include <../include/main.h>

int main(void) {

	t_log *logger;

	// ----------- CPU SERVER DE KERNEL ------------------------

	logger = iniciar_logger(rutaLog, nombreProceso, level);
	server_escuchar(logger, "4445");

	// ------------  CPU CLIENTE DE MEMORIA -------------------
	
	t_config *config =iniciar_config();
	logger = iniciar_logger("logCliente.log","cliente",LOG_LEVEL_INFO);

	conectarse(config, "IP", "PUERTO_MEMORIA", "cpu");
}
