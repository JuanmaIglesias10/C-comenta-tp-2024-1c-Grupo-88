#include <../include/main.h>

int main(void) {

	t_log *logger;
    // ------------ MEMORIA SERVER DE CPU -------------

	logger = iniciar_logger("logMemoria.log", "Server", LOG_LEVEL_INFO);
	server_escuchar(logger, "8003");
}