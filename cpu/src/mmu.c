/*MMU*/
#include "mmu.h"

uint32_t obtener_numero_pagina(uint32_t direccionLogica){
		return floor (direccionLogica/tam_pagina);
}

uint32_t obtener_desplazamiento_pagina(uint32_t direccionLogica){
	uint32_t numero_pagina = obtener_numero_pagina(direccionLogica);
	return (direccionLogica - numero_pagina * tam_pagina);
}

uint32_t calcular_direccion_fisica(uint32_t direccion_logica){
	uint32_t nro_pagina = obtener_numero_pagina(direccion_logica); //floor(dir_logica/tam_pagina)   				0
	uint32_t desplazamiento = obtener_desplazamiento_pagina(direccion_logica); //direccionLogica - numero_pagina * tam_pagina                0
	uint32_t nro_marco_memoria;

	// si no esta activada la TLB:

	if(config_cpu.cantidad_entradas_tlb == 0){
		nro_marco_memoria = pedir_marco_memoria(nro_pagina);
		log_info(logger_cpu, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pid_de_cde_ejecutando, nro_pagina, nro_marco_memoria); //log_obligatorio
		return nro_marco_memoria * tam_pagina + desplazamiento; // retorna la direccion_fisica
	}

	// si esta activada la TLB:

	uint32_t nro_marco_tlb = consultar_TLB(nro_pagina);

	// si esta la pag en la TLB:

	if (nro_marco_tlb != 1000) {
		log_info(logger_cpu, "PID: %d - TLB HIT - Pagina: %d", pid_de_cde_ejecutando, nro_pagina); //log_obligatorio
		log_info(logger_cpu, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pid_de_cde_ejecutando, nro_pagina, nro_marco_tlb); //log_obligatorio
		return nro_marco_tlb * tam_pagina + desplazamiento; // retorna la direccion_fisica
	}

	// si no esta la pag en la TLB:

	log_info(logger_cpu, "PID: %d - TLB MISS - Pagina: %d", pid_de_cde_ejecutando, nro_pagina); //log_obligatorio
	nro_marco_memoria = pedir_marco_memoria(nro_pagina);
	log_info(logger_cpu, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pid_de_cde_ejecutando, nro_pagina, nro_marco_memoria); //log_obligatorio

	actualizar_TLB(nro_pagina , nro_marco_memoria);

	return nro_marco_memoria * tam_pagina + desplazamiento; // retorna la direccion_fisica

	return -1; //Chequear, no deberia llegar hasta aquí, es para evitar el WARNING
}

uint32_t pedir_marco_memoria(uint32_t nro_pagina){
	// variables globales necesarias: pid_de_cde_ejecutando
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
		return nro_marco_recibido;
	} else log_error(logger_cpu, "error al obtener nro de marco desde memoria");
	
	return 999999;
}

uint32_t consultar_TLB(uint32_t nro_pagina) {



	for (size_t i = 0; i < list_size(lista_TLB); i++)
	{
		t_entrada_tlb* entrada_tlb = list_get(lista_TLB, i);
		if(nro_pagina  == entrada_tlb->nro_pagina && entrada_tlb->pid == pid_de_cde_ejecutando){
				//ACTUALIZAR CONTADOR LRU EN HIT
				if(es_lru()){
					entrada_tlb->cont_lru = cont_lru;
					cont_lru++;
				}
				return entrada_tlb->nro_marco;
			}
	}
	return 1000;
}

void actualizar_TLB(uint32_t nro_pagina,uint32_t nro_marco) {

	t_entrada_tlb* entrada_TLB_nueva = malloc(sizeof(t_entrada_tlb));
	entrada_TLB_nueva->pid = pid_de_cde_ejecutando;
	entrada_TLB_nueva->nro_marco = nro_marco;
	entrada_TLB_nueva->nro_pagina = nro_pagina;

	if(list_size(lista_TLB) < config_cpu.cantidad_entradas_tlb){
		list_add(lista_TLB,entrada_TLB_nueva);
	}

	else {
		if(es_fifo()){
			t_entrada_tlb* entrada_removida_fifo = list_remove(lista_TLB,0);
			list_add(lista_TLB,entrada_TLB_nueva);
		} 
		
		if(es_lru()) {
			int primer_contador = 100000;
			int indice_a_reemplazar;
			for (size_t i = 0; i < list_size(lista_TLB); i++)
			{
				t_entrada_tlb* entrada = list_get(lista_TLB , i);
				if(entrada->cont_lru < primer_contador){
					indice_a_reemplazar = i;
					primer_contador = entrada->cont_lru;
				}
			}
			entrada_TLB_nueva->cont_lru = cont_lru++;

			t_entrada_tlb* entrada_removida_lru = list_get(lista_TLB,indice_a_reemplazar);
			list_replace(lista_TLB, indice_a_reemplazar, entrada_TLB_nueva);
		}
	}
}


bool es_lru(){
	return strcmp(config_cpu.algoritmo_tlb,"LRU") == 0;
}

bool es_fifo(){
	return strcmp(config_cpu.algoritmo_tlb,"FIFO") == 0;
}