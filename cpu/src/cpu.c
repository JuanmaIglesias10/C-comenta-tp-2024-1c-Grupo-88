#include "cpu.h"

int main(void) {

	inicializar_cpu();

}

void inicializar_cpu() {

	logger_cpu = iniciar_logger("logCPU.log","CPU",LOG_LEVEL_INFO);
	inicializar_config();
    inicializar_registros();
	inicializar_conexiones();

}

void inicializar_config(){
	config = iniciar_config("./cpu.config");
	config_cpu.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	config_cpu.puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
	config_cpu.puerto_escucha_dispatch = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
	config_cpu.puerto_escucha_interrupt = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");
	config_cpu.cantidad_entradas_tlb = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
	config_cpu.algoritmo_tlb = config_get_string_value(config, "ALGORITMO_TLB");
}

void inicializar_conexiones(){
	fd_memoria = conectarse(config_cpu.ip_memoria, config_cpu.puerto_memoria, "MEMORIA", logger_cpu);

	fd_cpu_dis = iniciar_servidor(config_cpu.puerto_escucha_dispatch, logger_cpu);

	fd_cpu_int = iniciar_servidor(config_cpu.puerto_escucha_interrupt, logger_cpu);

	fd_kernel_dis = esperar_cliente(fd_cpu_dis, logger_cpu,"KERNEL(DISPATCH)"); 

	fd_kernel_int = esperar_cliente(fd_cpu_int, logger_cpu,"KERNEL(INTERRUPT)"); 
	
	//Esto posiblemente haya que quitarlo
	pthread_t hilo_cpu_memoria;
	pthread_create(&hilo_cpu_memoria, NULL, (void*)atender_memoria, NULL);
	pthread_detach(hilo_cpu_memoria);

	pthread_t hilo_kernel_dis;
	pthread_create(&hilo_kernel_dis, NULL, (void*)atender_kernel_dis, NULL);
	pthread_detach(hilo_kernel_dis);

	pthread_t hilo_kernel_int;
	pthread_create(&hilo_kernel_int, NULL, (void*)atender_kernel_int, NULL);
	pthread_join(hilo_kernel_int, NULL);


	// close(fd_memoria);
	// close(fd_cpu_dis);
	// close(fd_cpu_int);
	// close(fd_kernel_dis);
	// close(fd_kernel_int);

	// log_destroy(logger_cpu);
}

/*
	TO DO -> Creo que conviene hacerla despues de codear el ciclo basico de instruccion
			 y las operaciones para este check. Tambien despues de tener en claro como 
			 funcionan las interrupciones
*/
void inicializar_registros(){
    registros_cpu = malloc(sizeof(t_registros));
    
    registros_cpu->PC = 0;
    registros_cpu->AX = 0;
    registros_cpu->BX = 0;
    registros_cpu->CX = 0;
    registros_cpu->DX = 0;
    registros_cpu->EAX = 0;
    registros_cpu->EBX = 0;
    registros_cpu->ECX = 0;
    registros_cpu->EDX = 0;
    registros_cpu->SI = 0;
    registros_cpu->DI = 0;
}


void cargar_registros(t_cde* cde){
    registros_cpu->PC = cde->registros->PC;
    registros_cpu->AX = cde->registros->AX;
    registros_cpu->BX = cde->registros->BX;
    registros_cpu->CX = cde->registros->CX;
    registros_cpu->DX = cde->registros->DX;
    registros_cpu->EAX = cde->registros->EAX;
    registros_cpu->EBX = cde->registros->EBX;
    registros_cpu->ECX = cde->registros->ECX;
    registros_cpu->EDX = cde->registros->EDX;
    registros_cpu->EDX = cde->registros->SI;
    registros_cpu->EDX = cde->registros->DI;
}

void ejecutar_proceso(t_cde* cde){
	cargar_registros(cde);
    t_instruccion* instruccion_a_ejecutar;
    while(1){ //Por ahora no tenemos interrupciones, interrupciones por consola ni desalojo

        //Pedir a memoria la instruccion pasandole el pid y el pc
        log_info(logger_cpu, "PID: %d - FETCH - Program Counter: %d", cde->pid, registros_cpu->PC); //Obligatorio, si lo quitas te pega facu

        enviar_codOp(fd_memoria, PEDIDO_INSTRUCCION);
        t_buffer* buffer = crear_buffer();
        agregar_buffer_uint32(buffer, cde->pid);
        agregar_buffer_uint32(buffer, registros_cpu->PC); //En la siguiente iteracion, el pc es + 1
        enviar_buffer(buffer, fd_memoria);
        destruir_buffer(buffer);
        
        registros_cpu->PC++;
        t_buffer* buffer_recibido = recibir_buffer(fd_memoria);
        instruccion_a_ejecutar = leer_buffer_instruccion(buffer_recibido);
        if (!instruccion_a_ejecutar) {
            log_error(logger_cpu, "Error al leer la instrucción del buffer recibido.");
            continue;
        }
        destruir_buffer(buffer_recibido);
        log_info(logger_cpu,"%d %s %s %s %s %s",instruccion_a_ejecutar->codigo,instruccion_a_ejecutar->par1,instruccion_a_ejecutar->par2,instruccion_a_ejecutar->par4,instruccion_a_ejecutar->par4,instruccion_a_ejecutar->par5	);
        // pthread_mutex_lock(&mutex_instruccion_actualizada); Mirar luego esto, por ahora que la chupe
        // instruccion_actualizada = instruccion_a_ejecutar->codigo;
        // pthread_mutex_unlock(&mutex_instruccion_actualizada);
        ejecutar_instruccion(cde, instruccion_a_ejecutar);
    }
} 

void ejecutar_instruccion(t_cde* cde, t_instruccion* instruccion_a_ejecutar){
    // uint32_t par1;
    // uint32_t par2;
    switch(instruccion_a_ejecutar->codigo){
        case SET:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // par2 = leerEnteroParametroInstruccion(2, instruccion_a_ejecutar);
            int prueba = atoi(instruccion_a_ejecutar->par2);
            if (prueba < 255){
                ejecutar_set8(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            } else {
                ejecutar_set32(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            }
            // if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
            destruir_instruccion(instruccion_a_ejecutar);
            break;
        case MOV_IN:
            // log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // par2 = leerEnteroParametroInstruccion(2, instruccion_a_ejecutar);
            // ejecutar_mov_in(instruccion_a_ejecutar->par1, par2, cde);
            // if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
            //     destruir_instruccion(instruccion_a_ejecutar);
            // break;
        case MOV_OUT:
            // log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // par1 = leerEnteroParametroInstruccion(1, instruccion_a_ejecutar);
            // ejecutar_mov_out(par1, instruccion_a_ejecutar->par2, cde);
            // if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
            //     destruir_instruccion(instruccion_a_ejecutar);
            // break;
        case SUM:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            ejecutar_sum(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
                destruir_instruccion(instruccion_a_ejecutar);
            break;
        case SUB:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            ejecutar_sub(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            //if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
                destruir_instruccion(instruccion_a_ejecutar);
            break;
        case JNZ:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // par2 = leerEnteroParametroInstruccion(2, instruccion_a_ejecutar);
            ejecutar_jnz(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
                destruir_instruccion(instruccion_a_ejecutar);
            break;
        case RESIZE:
			//
            break;
		case COPY_STRING:
			//
			break;
        case WAIT:
            // log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1);
            // ejecutar_wait(instruccion_a_ejecutar->par1);
            // if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
            //     destruir_instruccion(instruccion_a_ejecutar);
            // break;
        case SIGNAL:
            // log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1);
            // ejecutar_signal(instruccion_a_ejecutar->par1);
            // if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
            //     destruir_instruccion(instruccion_a_ejecutar);
            // break;
        case IO_GEN_SLEEP:
            //
            break;
        case IO_STDIN_READ:
            // 
        case IO_STDOUT_WRITE:
			//
            break;
        case IO_FS_CREATE:
			//
            break;
        case IO_FS_DELETE:
			//
            break;
        case IO_FS_TRUNCATE:
            //
        case IO_FS_WRITE:
            //
        case IO_FS_READ:
            //
        case EXIT:
            //log_info(logger_cpu, "PID: %d - Ejecutando: %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar));
            //ejecutar_exit();
            break;
        default:
            log_warning(logger_cpu, "Instruccion no reconocida");
            break;
    }
}

void destruir_instruccion(t_instruccion* instruccion){
	free(instruccion->par1);
	free(instruccion->par2);
	free(instruccion->par3);
	free(instruccion->par4);
	free(instruccion->par5);
	free(instruccion);
}