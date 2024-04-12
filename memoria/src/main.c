#include <../include/main.h>

int main(void) {

	t_log *logger;
    // ------------ MEMORIA SERVER DE CPU -------------

	logger = iniciar_logger(rutaLog, nombreProceso, level);
	server_escuchar(logger, "8003");

}