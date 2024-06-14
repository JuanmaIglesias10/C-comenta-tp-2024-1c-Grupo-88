#include "cpu.h"

int main(void) {

	inicializar_cpu();

}

void inicializar_cpu() {

	logger_cpu = iniciar_logger("logCPU.log","CPU",LOG_LEVEL_INFO);
	inicializar_config();
    inicializar_registros();
	inicializar_conexiones();
    inicializar_semaforos();

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

void inicializar_semaforos(){
    pthread_mutex_init(&mutex_realizar_desalojo, NULL);
    pthread_mutex_init(&mutex_cde_ejecutando,NULL);
    pthread_mutex_init(&mutex_instruccion_actualizada,NULL);
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
    registros_cpu->SI = cde->registros->SI;
    registros_cpu->DI = cde->registros->DI;
}

void guardar_cde(t_cde* cde){
    cde->registros->PC = registros_cpu->PC;
    cde->registros->AX = registros_cpu->AX;
    cde->registros->BX = registros_cpu->BX;
    cde->registros->CX = registros_cpu->CX;
    cde->registros->DX = registros_cpu->DX;
    cde->registros->EAX = registros_cpu->EAX;
    cde->registros->EBX = registros_cpu->EBX;
    cde->registros->ECX = registros_cpu->ECX;
    cde->registros->EDX = registros_cpu->EDX;
    cde->registros->SI = registros_cpu->SI;
    cde->registros->DI = registros_cpu->DI;
}


// Tuve que mover esto aca!! necesito la variable global instruccion_actualizada para ver si es bloqueante! 
void* atender_kernel_int()
{
	while (1) {
		mensajeKernelCpu cod_op = recibir_codOp(fd_kernel_int);
		switch (cod_op) {
            case DESALOJO:
                t_buffer* buffer = recibir_buffer(fd_kernel_int); // recibe pid o lo que necesite
                uint32_t pid_recibido = leer_buffer_uint32(buffer);
                // se desaloja proceso en ejecucion
                if(algoritmo_planificacion == 1 && pid_de_cde_ejecutando != pid_recibido){
                    break;
                }
                else if(algoritmo_planificacion == 1 && pid_de_cde_ejecutando == pid_recibido){

                     if(es_bloqueante(instruccion_actualizada)){ 
                        break;
                    }
                }
                pthread_mutex_lock(&mutex_realizar_desalojo);
                realizar_desalojo = 1;
                pthread_mutex_unlock(&mutex_realizar_desalojo);
                break;
            default:
                log_warning(logger_cpu, "Se desconectó KERNEL (INTERRUPT)");
                return NULL;
            }
	}
}


void ejecutar_proceso(t_cde* cde){
	cargar_registros(cde);
    t_instruccion* instruccion_a_ejecutar;
    while(interrupcion != 1 && realizar_desalojo != 1){

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
        /*
        PARA CHEQUEAR QUE ESTOY RECIBIENDO LA INSTRUCCIONES, NO QUITAR!!
        log_info(logger_cpu,"%d %s %s %s %s %s",instruccion_a_ejecutar->codigo,instruccion_a_ejecutar->par1,instruccion_a_ejecutar->par2,instruccion_a_ejecutar->par4,instruccion_a_ejecutar->par4,instruccion_a_ejecutar->par5	);
        */
        
         pthread_mutex_lock(&mutex_instruccion_actualizada); 
         instruccion_actualizada = instruccion_a_ejecutar->codigo;
         pthread_mutex_unlock(&mutex_instruccion_actualizada);
        ejecutar_instruccion(cde, instruccion_a_ejecutar);
    }
    if(interrupcion){
        interrupcion = 0;
        // pthread_mutex_lock(&mutex_interrupcion_consola);
        // interrupcion_consola = 0;
        // pthread_mutex_unlock(&mutex_interrupcion_consola);
        pthread_mutex_lock(&mutex_realizar_desalojo);
        realizar_desalojo = 0;
        pthread_mutex_unlock(&mutex_realizar_desalojo);
        log_info(logger_cpu, "PID: %d - Volviendo a kernel por instruccion %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar));
        desalojar_cde(cde, instruccion_a_ejecutar);
    } else if (realizar_desalojo){
        interrupcion = 0;
        // pthread_mutex_lock(&mutex_interrupcion_consola);
        // interrupcion_consola = 0;
        // pthread_mutex_unlock(&mutex_interrupcion_consola);
        pthread_mutex_lock(&mutex_realizar_desalojo);
        realizar_desalojo = 0;
        pthread_mutex_unlock(&mutex_realizar_desalojo);
        if(algoritmo_planificacion == 1) // significa que es RR
            log_info(logger_cpu, "PID: %d - Desalojado por fin de Quantum", cde->pid); 
        else if(algoritmo_planificacion == 2) // significa que es VRR
            log_info(logger_cpu, "PID: %d - Desalojado por fin de Quantum VRR", cde->pid);
        desalojar_cde(cde, instruccion_a_ejecutar);
    }
} 

void ejecutar_instruccion(t_cde* cde, t_instruccion* instruccion_a_ejecutar){
    // uint32_t par1;
    // uint32_t par2;
    switch(instruccion_a_ejecutar->codigo){
        case SET:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // par2 = leerEnteroParametroInstruccion(2, instruccion_a_ejecutar);
            if ( strcmp(instruccion_a_ejecutar->par1,"AX") == 0 || strcmp(instruccion_a_ejecutar->par1,"BX") == 0 || strcmp(instruccion_a_ejecutar->par1,"CX") == 0 || strcmp(instruccion_a_ejecutar->par1,"DX") == 0 ){
                ejecutar_set8(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            } else {
                ejecutar_set32(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            }
            if (interrupcion == 0 && realizar_desalojo == 0)
                destruir_instruccion(instruccion_a_ejecutar);
            break;
        case MOV_IN:
            // log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // par2 = leerEnteroParametroInstruccion(2, instruccion_a_ejecutar);
            // ejecutar_mov_in(instruccion_a_ejecutar->par1, par2, cde);
                        if (interrupcion == 0 && realizar_desalojo == 0)

                destruir_instruccion(instruccion_a_ejecutar);
            break;
        case MOV_OUT:
            // log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // par1 = leerEnteroParametroInstruccion(1, instruccion_a_ejecutar);
            // ejecutar_mov_out(par1, instruccion_a_ejecutar->par2, cde);
            // if (interruption == 0 && realizar_desalojo == 0 && interrupcion_consola == 0)
            //     destruir_instruccion(instruccion_a_ejecutar);
            break;
        case SUM:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            ejecutar_sum(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            if (interrupcion == 0 && realizar_desalojo == 0)
                 destruir_instruccion(instruccion_a_ejecutar);
            break;
        case SUB:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            ejecutar_sub(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
              if (interrupcion == 0 && realizar_desalojo == 0)
                destruir_instruccion(instruccion_a_ejecutar);
            break;
        case JNZ:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            // par2 = leerEnteroParametroInstruccion(2, instruccion_a_ejecutar);
            ejecutar_jnz(instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            if (interrupcion == 0 && realizar_desalojo == 0)
                destruir_instruccion(instruccion_a_ejecutar);
            break;
        case RESIZE:
			//
            break;
		case COPY_STRING:
			//
			break;
        case WAIT:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1);
            ejecutar_wait(instruccion_a_ejecutar->par1);
            if (interrupcion == 0 && realizar_desalojo == 0)
                destruir_instruccion(instruccion_a_ejecutar);
            break;
        case SIGNAL:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1);
            ejecutar_signal(instruccion_a_ejecutar->par1);
            if (interrupcion == 0 && realizar_desalojo == 0)
                 destruir_instruccion(instruccion_a_ejecutar);
            break;
        case IO_GEN_SLEEP:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s - %s %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar), instruccion_a_ejecutar->par1, instruccion_a_ejecutar->par2);
            ejecutar_io_gen_sleep(instruccion_a_ejecutar->par1,instruccion_a_ejecutar->par2); //(uint32_t)(uintptr_t)
                if (interrupcion == 0 && realizar_desalojo == 0)
                    destruir_instruccion(instruccion_a_ejecutar);
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
            break;
        case IO_FS_WRITE:
            //
            break;
        case IO_FS_READ:
            //
            break;
        case EXIT:
            log_info(logger_cpu, "PID: %d - Ejecutando: %s", cde->pid, obtener_nombre_instruccion(instruccion_a_ejecutar));
            ejecutar_exit();
            break;
        default:
            log_warning(logger_cpu, "Instruccion no reconocida");
            break;
    }
}

bool es_bloqueante(t_codigo_instruccion instruccion){
    switch(instruccion){
    case SET:
        return false;
        break;
    case MOV_IN:
        //
        break;
    case MOV_OUT:
        //
        break;
    case SUM:
        return false;
        break;
    case SUB:
        return false;
        break;
    case JNZ:
        return false;
        break;
    case RESIZE:
        //
        break;
    case COPY_STRING:
        //
        break;
    case WAIT: 
        return true;
        break;
    case SIGNAL:
        return true;
        break;
    case IO_STDIN_READ:
        //
        break;
    case IO_STDOUT_WRITE:
        //
        break;
    case IO_FS_CREATE:
        //
        break;
    case IO_FS_DELETE:
        //
        break;
    case IO_FS_WRITE: 
        //
        break;
    case IO_FS_READ:
        //
        break;
    case IO_FS_TRUNCATE:
        //
        break;
    case IO_GEN_SLEEP:
        return true;
        break;
    case EXIT:
        return true;
        break;
    default:
        return false;
        break;
    }
}

void desalojar_cde(t_cde* cde, t_instruccion* instruccion_a_ejecutar){
    
    guardar_cde(cde); //cargar registros de cpu en el cde
    devolver_cde_a_kernel(cde, instruccion_a_ejecutar);
    destruir_cde(cde);
    
    pthread_mutex_lock(&mutex_cde_ejecutando);
    pid_de_cde_ejecutando = UINT32_MAX;
    pthread_mutex_unlock(&mutex_cde_ejecutando);
    
    pthread_mutex_lock(&mutex_instruccion_actualizada);
    instruccion_actualizada = NULO_INST;
    pthread_mutex_unlock(&mutex_instruccion_actualizada);

    destruir_instruccion(instruccion_a_ejecutar);
}

void devolver_cde_a_kernel(t_cde* cde, t_instruccion* instruccion_a_ejecutar){

    t_buffer* buffer = crear_buffer();
    agregar_buffer_cde(buffer, cde);
    // if(strcmp(instruccion_a_ejecutar->par1, "") == 0 ){
    //     instruccion_a_ejecutar->par1 = NULL;
    // } else if (strcmp(instruccion_a_ejecutar->par2, "") == 0){
    //     instruccion_a_ejecutar->par2 = NULL;
    // } else if (strcmp(instruccion_a_ejecutar->par3, "") == 0){
    //     instruccion_a_ejecutar->par3 = NULL;
    // } else if (strcmp(instruccion_a_ejecutar->par4, "") == 0){
    //     instruccion_a_ejecutar->par4 = NULL;
    // }
    //  else if (strcmp(instruccion_a_ejecutar->par5, "") == 0){
    //     instruccion_a_ejecutar->par5 = NULL;
    // }
    agregar_buffer_instruccion(buffer, instruccion_a_ejecutar);
/*
    // caso de page fault, tiene que volver a kernel con el nroPagina que genero el page fault
    if(instruccion_a_ejecutar->codigo == MOV_IN){
        uint32_t dirLogica = leerEnteroParametroInstruccion(2, instruccion_a_ejecutar);
        uint32_t nroPagina = obtener_numero_pagina(dirLogica);
        buffer_write_uint32(buffer, nroPagina);
    }
    else if(instruccion_a_ejecutar->codigo == MOV_OUT){
        uint32_t dirLogica = leerEnteroParametroInstruccion(1, instruccion_a_ejecutar);
        uint32_t nroPagina = obtener_numero_pagina(dirLogica);
        buffer_write_uint32(buffer, nroPagina);
    }
    // caso de F_WRITE y F_READ, devuelve a kernel la direccion fisica
    else if(instruccion_a_ejecutar->codigo == F_READ || instruccion_a_ejecutar->codigo == F_WRITE){
        uint32_t dirLogica = leerEnteroParametroInstruccion(2, instruccion_a_ejecutar);
        if(pf_con_funcion_fs){
            uint32_t nroPagina = obtener_numero_pagina(dirLogica);
            buffer_write_uint8(buffer, HAY_PAGE_FAULT);
            buffer_write_uint32(buffer, nroPagina);
            pf_con_funcion_fs = 0;
        }
        else{
            uint32_t nroPagina = obtener_numero_pagina(dirLogica);
            uint32_t dirFisica = calcular_direccion_fisica(dirLogica, cde);
            buffer_write_uint8(buffer, DIRECCION_FISICA_OK);
            buffer_write_uint32(buffer, dirFisica);
            buffer_write_uint32(buffer, nroPagina);
        }
    }
*/

    enviar_buffer(buffer, fd_kernel_dis);
    destruir_buffer(buffer);
}

