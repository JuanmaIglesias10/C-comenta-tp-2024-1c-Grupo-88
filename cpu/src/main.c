#include <../include/main.h>

int main(void) {

	t_log *logger;

	// ------------  CPU ------> MEMORIA -------------------
	
	t_config* config = iniciar_config("../cpu.config");
	logger = iniciar_logger("logCPU.log","Client",LOG_LEVEL_INFO);

	conectarse(config, "IP", "PUERTO_MEMORIA", "CPU", logger);

	// ----------- KERNEL ------> CPU ------------------------

	logger = iniciar_logger("logCPU.log", "Server", LOG_LEVEL_DEBUG);
	server_escuchar(logger, "4445");
}

