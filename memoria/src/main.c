#include <../include/main.h>

int main(void) {
    // ------------ MEMORIA SERVER DE CPU-KERNEL-ENTRADASALIDA -------------

	t_log* logger = iniciar_logger("logMemoria.log", "Server", LOG_LEVEL_DEBUG);
	server_escuchar(logger, "8003");
}