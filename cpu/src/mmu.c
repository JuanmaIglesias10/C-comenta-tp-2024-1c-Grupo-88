/*MMU*/
#include "mmu.h"

int obtener_nro_pagina(int direccionLogica){
		return floor (direccionLogica/tam_pagina);
}

int obtener_desplazamiento_pagina(int direccionLogica){
	int numero_pagina = obtener_nro_pagina(direccionLogica);
	return (direccionLogica - numero_pagina * tam_pagina);
}

uint32_t calcular_direccion_fisica(int direccion_logica, t_cde* cde){
	int nro_pagina = obtener_nro_pagina(direccion_logica);
	int desplazamiento = obtener_desplazamiento_pagina(direccion_logica);
    enviar_codOp(fd_memoria,NUMERO_MARCO_SOLICITUD);

    t_buffer* buffer = crear_buffer();
	agregar_buffer_uint32(buffer, nro_pagina);
	agregar_buffer_uint32(buffer, cde->pid);
	enviar_buffer(buffer, fd_memoria);
	destruir_buffer(buffer);

	mensajeCpuMemoria codigo_recibido = recibir_codOp(fd_memoria);

	if(codigo_recibido == NUMERO_MARCO_OK){
		buffer = recibir_buffer(fd_memoria);
		uint32_t nro_marco_recibido = leer_buffer_uint32(buffer);
		destruir_buffer(buffer);
		log_info(logger_cpu, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", cde->pid, nro_pagina, nro_marco_recibido); //log_obligatorio
		return nro_marco_recibido * tam_pagina + desplazamiento; // retorna la direccion_fisica
	}
	else if(codigo_recibido == PAGE_FAULT){
		log_info(logger_cpu, "Page Fault PID: %d - Página: %d", cde->pid, nro_pagina);
        // interruption = 1; // devolvemos a kernel el cde del proceso en ejecucion 
		cde->registros->PC--;
		return -1;
	}	
}