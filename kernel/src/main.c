#include <stdlib.h>
#include <stdio.h>
#include </home/utnso/tp-2024-1c-Grupo-88/kernel/include/main.h>
#include </home/utnso/tp-2024-1c-Grupo-88/utils/include/protocolo.h>
#include </home/utnso/tp-2024-1c-Grupo-88/utils/include/sockets.h>

int main(void) {
    
    int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	// log_info(logger,"Hola! Soy un log");
	

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	conexion = crear_conexion(ip, puerto);

	enviar_mensaje("hola, soy el kernel",conexion);
	liberar_conexion(conexion);
	config_destroy(config);
}
t_config* iniciar_config(){
	t_config* nuevo_config = config_create("/home/utnso/tp-2024-1c-Grupo-88/kernel/kernel.config");
	if(nuevo_config == NULL){
		perror("Hubo un error en las CONFIG");
		exit(EXIT_FAILURE);
	}
	return nuevo_config;
}