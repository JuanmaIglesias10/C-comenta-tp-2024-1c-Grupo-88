/*MMU*/
#include "mmu.h"

int obtener_numero_pagina(int direccionLogica){
		return floor (direccionLogica/tam_pagina);
}

int obtener_desplazamiento_pagina(int direccionLogica){
	int numero_pagina = obtener_numero_pagina(direccionLogica);
	return (direccionLogica - numero_pagina * tam_pagina);
}

uint32_t calcular_direccion_fisica(int direccion_logica){
	int nro_pagina = obtener_numero_pagina(direccion_logica); //floor(dir_logica/tam_pagina)   				0
	int desplazamiento = obtener_desplazamiento_pagina(direccion_logica); //direccionLogica - numero_pagina * tam_pagina                0
	
    enviar_codOp(fd_memoria,NUMERO_MARCO_SOLICITUD); //Hacia atender_cpu en atender_memoria.c

    t_buffer* buffer = crear_buffer();
	agregar_buffer_uint32(buffer, nro_pagina); //0
	agregar_buffer_uint32(buffer, pid_de_cde_ejecutando);
	enviar_buffer(buffer, fd_memoria);
	destruir_buffer(buffer);

	mensajeCpuMemoria codigo_recibido = recibir_codOp(fd_memoria);
	if(codigo_recibido == NUMERO_MARCO_OK){ //Desde memoria.c en devolver_nro_marco
		buffer = recibir_buffer(fd_memoria);
		uint32_t nro_marco_recibido = leer_buffer_uint32(buffer); //0
		destruir_buffer(buffer);
		log_info(logger_cpu, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pid_de_cde_ejecutando, nro_pagina, nro_marco_recibido); //log_obligatorio
		return nro_marco_recibido * tam_pagina + desplazamiento; // retorna la direccion_fisica
	}
	return -1; //Chequear, no deberia llegar hasta aquí, es para evitar el WARNING
}