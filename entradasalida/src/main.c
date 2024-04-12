#include <stdlib.h>
#include <stdio.h>
#include <../utils/include/protocolo.h>
#include <../utils/include/sockets.h>

int main(void) {


	// ------------  ENTRADA/SALIDA CLIENTE -------------------
	
	t_config *config =iniciar_config();
	logger = iniciar_logger("logCliente.log","cliente",LOG_LEVEL_INFO);

	conectarse(config, "IP", "PUERTO_MEMORIA", "cpu");
}
