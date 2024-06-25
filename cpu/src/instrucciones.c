#include "instrucciones.h"


char* obtener_nombre_instruccion(t_instruccion* instruccion) {
    switch(instruccion->codigo) {
        case SET:
            return "SET";
            break;
	    case MOV_IN:
            return "MOV_IN ";
            break;
	    case MOV_OUT:
            return "MOV_OUT";
            break;
	    case SUM:
            return "SUM";
            break;
	    case SUB:
            return "SUB";
            break;
	    case JNZ:
            return "JNZ";
            break;
	    case RESIZE:
            return "RESIZE";
            break;
	    case OUT_OF_MEMORY_VUELTA:
            return "RESIZE";
            break;
	    case COPY_STRING:
            return "COPY_STRING";
            break;
	    case WAIT:
            return "WAIT";
            break;
	    case SIGNAL:
            return "SIGNAL";
            break;
	    case IO_GEN_SLEEP:
            return "IO_GEN_SLEEP";
            break;
	    case IO_STDIN_READ:
            return "IO_STDIN_READ";
            break;
	    case IO_STDOUT_WRITE:
            return "IO_STDOUT_WRITE";
            break;
	    case IO_FS_CREATE:
            return "IO_FS_CREATE";
            break;
	    case IO_FS_DELETE:
            return "IO_FS_DELETE";
            break;
		case IO_FS_TRUNCATE:
            return "IO_FS_TRUNCATE";
            break;
	    case IO_FS_WRITE:
            return "IO_FS_WRITE";
            break;
	    case IO_FS_READ:
            return "IO_FS_READ";
            break;
	    case EXIT:
            return "EXIT";
            break;
        default:
            return "Instruccion desconocida";
            break;
    }
}

// FUNCIONALIDAD DE LAS INSTRUCCIONES

void ejecutar_set8(char* registro, uint8_t valor_recibido) {
    if (strcmp(registro, "AX") == 0) {
        registros_cpu->AX = valor_recibido;
    }
    else if (strcmp(registro, "BX") == 0) {
        registros_cpu->BX = valor_recibido;
    }
    else if (strcmp(registro, "CX") == 0) {
        registros_cpu->CX = valor_recibido;
    }
    else if (strcmp(registro, "DX") == 0) {
        registros_cpu->DX = valor_recibido;
    }
    else {
        log_error(logger_cpu, "No se reconoce el registro %s", registro);
    }
}

void ejecutar_set32(char* registro, uint32_t valor_recibido){
     if (strcmp(registro, "PC") == 0) { 
        registros_cpu->PC = valor_recibido;
    }
    else if (strcmp(registro, "EAX") == 0) {
        registros_cpu->EAX = valor_recibido;
    }
    else if (strcmp(registro, "EBX") == 0) {
        registros_cpu->EBX = valor_recibido;
    }
    else if (strcmp(registro, "ECX") == 0) {
        registros_cpu->ECX = valor_recibido;
    }
    else if (strcmp(registro, "EDX") == 0) {
        registros_cpu->EDX = valor_recibido;
    }
    else if (strcmp(registro, "SI") == 0) {
        registros_cpu->SI = valor_recibido;
    }
    else if (strcmp(registro, "DI") == 0) {
        registros_cpu->DI = valor_recibido;
    }
    else {
        log_error(logger_cpu, "No se reconoce el registro %s", registro);
    }
}

uint8_t  buscar_valor_registro8(void* registro) {
	uint8_t  valorLeido8;
	if(strcmp(registro, "AX") == 0){
		valorLeido8 = registros_cpu->AX;
        return valorLeido8;
	}
    else if(strcmp(registro, "BX") == 0){
		valorLeido8 = registros_cpu->BX;
        return valorLeido8;
	}
    else if(strcmp(registro, "CX") == 0){
		valorLeido8 = registros_cpu->CX;
        return valorLeido8;
	}
    else if(strcmp(registro, "DX") == 0){
		valorLeido8 = registros_cpu->DX;
        return valorLeido8;
	} else{
        return 100;
    }

}

uint32_t  buscar_valor_registro32(void* registro){
    uint32_t valorLeido32;
     if(strcmp(registro, "EAX") == 0){
            valorLeido32 = registros_cpu->EAX;
            return valorLeido32;
        }
    else if(strcmp(registro, "EBX") == 0){
        valorLeido32 = registros_cpu->EBX;
        return valorLeido32;
    }
    else if(strcmp(registro, "ECX") == 0){
        valorLeido32 = registros_cpu->ECX;
        return valorLeido32;
    }
    else if(strcmp(registro, "EDX") == 0){
        valorLeido32 = registros_cpu->EDX;
        return valorLeido32;
    } else{
        return 1000;
    }


}

void ejecutar_sum(char* reg_dest, char* reg_origen) {
    if(strcmp(reg_dest, "AX") == 0){
        uint8_t  valor_reg_origen = buscar_valor_registro8(reg_origen);
        registros_cpu->AX += valor_reg_origen;
    }    
    else if(strcmp(reg_dest, "BX") == 0){
        uint8_t valor_reg_origen = buscar_valor_registro8(reg_origen);
        registros_cpu->BX += valor_reg_origen;
    }
    else if(strcmp(reg_dest, "CX") == 0){
        uint8_t valor_reg_origen = buscar_valor_registro8(reg_origen);
        registros_cpu->CX += valor_reg_origen;
    }
    else if(strcmp(reg_dest, "DX") == 0){
        uint8_t valor_reg_origen = buscar_valor_registro8(reg_origen);
        registros_cpu->DX += valor_reg_origen;
    }
    else if(strcmp(reg_dest, "EAX") == 0){
        uint32_t valor_reg_origen = buscar_valor_registro32(reg_origen);
        registros_cpu->EAX += valor_reg_origen;
    }
    else if(strcmp(reg_dest, "EBX") == 0){
        uint32_t valor_reg_origen = buscar_valor_registro32(reg_origen);
        registros_cpu->EBX += valor_reg_origen;
    }
    else if(strcmp(reg_dest, "ECX") == 0){
        uint32_t valor_reg_origen = buscar_valor_registro32(reg_origen);
        registros_cpu->ECX += valor_reg_origen;
    }
    else if(strcmp(reg_dest, "EDX") == 0){
        uint32_t valor_reg_origen = buscar_valor_registro32(reg_origen);
        registros_cpu->EDX += valor_reg_origen;
    }
    else
        log_warning(logger_cpu, "Registro no valido");
}

void ejecutar_sub(char* reg_dest, char* reg_origen) {
    if(strcmp(reg_dest, "AX") == 0){
        uint8_t  valor_reg_origen = buscar_valor_registro8(reg_origen);
        registros_cpu->AX -= valor_reg_origen;
    }    
    else if(strcmp(reg_dest, "BX") == 0){
        uint8_t valor_reg_origen = buscar_valor_registro8(reg_origen);
        registros_cpu->BX -= valor_reg_origen;
    }
    else if(strcmp(reg_dest, "CX") == 0){
        uint8_t valor_reg_origen = buscar_valor_registro8(reg_origen);
        registros_cpu->CX -= valor_reg_origen;
    }
    else if(strcmp(reg_dest, "DX") == 0){
        uint8_t valor_reg_origen = buscar_valor_registro8(reg_origen);
        registros_cpu->DX -= valor_reg_origen;
    }
    else if(strcmp(reg_dest, "EAX") == 0){
        uint32_t valor_reg_origen = buscar_valor_registro32(reg_origen);
        registros_cpu->EAX -= valor_reg_origen;
    }
    else if(strcmp(reg_dest, "EBX") == 0){
        uint32_t valor_reg_origen = buscar_valor_registro32(reg_origen);
        registros_cpu->EBX -= valor_reg_origen;
    }
    else if(strcmp(reg_dest, "ECX") == 0){
        uint32_t valor_reg_origen = buscar_valor_registro32(reg_origen);
        registros_cpu->ECX -= valor_reg_origen;
    }
    else if(strcmp(reg_dest, "EDX") == 0){
        uint32_t valor_reg_origen = buscar_valor_registro32(reg_origen);
        registros_cpu->EDX -= valor_reg_origen;
    }
    else
        log_warning(logger_cpu, "Registro no valido");
}

void ejecutar_jnz(void* registro, char* char_nro_instruccion) {
    uint32_t nro_instruccion = atoi(char_nro_instruccion);
    if(strcmp(registro, "AX") == 0) {
        if(registros_cpu->AX != 0)
            registros_cpu->PC = nro_instruccion;
    }

    else if(strcmp(registro, "BX") == 0) {
        if(registros_cpu->BX != 0)
            registros_cpu->PC = nro_instruccion;
    }

    else if(strcmp(registro, "CX") == 0) {
        if(registros_cpu->CX != 0)
            registros_cpu->PC= nro_instruccion;
    }

    else if(strcmp(registro, "DX") == 0) {
        if(registros_cpu->DX != 0)
            registros_cpu->PC= nro_instruccion;
    }

    else if(strcmp(registro, "EAX") == 0) {
        if(registros_cpu->EAX != 0)
            registros_cpu->PC= nro_instruccion;
    }
    else if(strcmp(registro, "EBX") == 0) {
        if(registros_cpu->EBX != 0)
            registros_cpu->PC= nro_instruccion;
    }

    else if(strcmp(registro, "ECX") == 0) {
        if(registros_cpu->ECX != 0)
            registros_cpu->PC= nro_instruccion;
    }

    else if(strcmp(registro,"EDX") == 0) {
        if(registros_cpu->DX != 0)
            registros_cpu->PC = nro_instruccion;
    }

    else
        log_warning(logger_cpu, "Registro no valido");
}

void ejecutar_io_gen_sleep(char* nombreInterfaz, char* charUnidadesDeTrabajo) {
    uint32_t unidadesDeTrabajo = atoi(charUnidadesDeTrabajo);
    // log_info(logger_cpu, unidadesDeTrabajo);
    enviar_codOp(fd_kernel_int, INTERRUPT);
    t_buffer* buffer_a_enviar = crear_buffer();
    agregar_buffer_uint8(buffer_a_enviar,unidadesDeTrabajo);
    agregar_buffer_string(buffer_a_enviar,nombreInterfaz);
    enviar_buffer(buffer_a_enviar,fd_kernel_int);
    destruir_buffer(buffer_a_enviar);
    interrupcion = 1;
    
}

void ejecutar_wait(char* recurso){ //solicitar a kernel que se asigne una instancia del recurso
    interrupcion = 1;
}

void ejecutar_signal(char* recurso){ //solicitar a kernel que se libere una instancia del recurso
    interrupcion = 1;
}

void ejecutar_exit(){
    interrupcion = 1;
}

int ejecutar_resize(char* charTamanio){
    enviar_codOp(fd_memoria, RESIZE_SOLICITUD);
    uint32_t tamanio = atoi(charTamanio);
    t_buffer* buffer = crear_buffer();
    agregar_buffer_uint32(buffer, pid_de_cde_ejecutando);
    agregar_buffer_uint32(buffer, tamanio);
    enviar_buffer(buffer,fd_memoria);
    destruir_buffer(buffer);

    mensajeCpuMemoria cod_op = recibir_codOp(fd_memoria);
    if(cod_op == OUT_OF_MEMORY){
        interrupcion = 1;
        return 1;

    } else if (cod_op == RESIZE_OK){
        return 0;
    }
    return 255; //Esto no deberia suceder
}


void ejecutar_mov_in(char* registro, char* registroDirLogica){ //MOV_IN BX EAX -> EN BX ME PONE EL VALOR DE DIR LOGICA 0 (42)
    uint32_t dirLogica;

    if (es_uint8(registroDirLogica)){
        dirLogica = (uint32_t)buscar_valor_registro8(registroDirLogica);
    } else {
        dirLogica = buscar_valor_registro32(registroDirLogica);
    }

    uint32_t dirFisica = calcular_direccion_fisica(dirLogica);
    uint32_t numPagina = obtener_numero_pagina(dirLogica);

    enviar_codOp(fd_memoria, MOV_IN_SOLICITUD);
    
    t_buffer* buffer = crear_buffer();

    if (es_uint8(registro)){ //Envio primero el tamaño del registro
        agregar_buffer_uint32(buffer,8);
    }else {
        agregar_buffer_uint32(buffer,32);
    }
    agregar_buffer_uint32(buffer, dirFisica);
    agregar_buffer_uint32(buffer, pid_de_cde_ejecutando);
    agregar_buffer_uint32(buffer, numPagina);
    enviar_buffer(buffer, fd_memoria);
    destruir_buffer(buffer);
    
    mensajeCpuMemoria codigoMemoria = recibir_codOp(fd_memoria);
    uint8_t valorLeido8;
    uint32_t valorLeido32;

    buffer = recibir_buffer(fd_memoria);
    if (es_uint8(registro)) {
        valorLeido8 = leer_buffer_uint8(buffer);
    } else {
        valorLeido32 = leer_buffer_uint32(buffer);
    }
    destruir_buffer(buffer);
    
    if (codigoMemoria == MOV_IN_OK) 
        log_info(logger_cpu,"PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d", pid_de_cde_ejecutando, dirFisica, es_uint8(registro) ? valorLeido8 : valorLeido32);

    if (es_uint8(registro)) {
        ejecutar_set8(registro, valorLeido8);
        log_warning(logger_cpu, "%d", valorLeido8);
    } else {
        ejecutar_set32(registro, valorLeido32);
    }
}

void ejecutar_mov_out(char* registroDirLogica, char* registro){ //MOV_OUT EAX AX -> MOV_OUT 0 42
    uint32_t dirLogica;
    if (es_uint8(registroDirLogica)){
        dirLogica = (uint32_t)buscar_valor_registro8(registroDirLogica);
    } else {
        dirLogica = buscar_valor_registro32(registroDirLogica); //0
    }

    uint8_t valorAEscribir8;
    uint32_t valorAEscribir32;

    if (es_uint8(registro)){
        valorAEscribir8 = buscar_valor_registro8(registro); //42
    } else {
        valorAEscribir32 = buscar_valor_registro32(registro);
    }
    
    uint32_t dirFisica = calcular_direccion_fisica(dirLogica); //0
    uint32_t numPagina = obtener_numero_pagina(dirLogica); //0

    enviar_codOp(fd_memoria, MOV_OUT_SOLICITUD);
    
    // Le manda a memoria la df y el valor del registro encontrado
    t_buffer* buffer = crear_buffer();
    agregar_buffer_uint32(buffer, dirFisica);

    if (es_uint8(registro)){
        agregar_buffer_uint32(buffer,8);
        agregar_buffer_uint8(buffer, valorAEscribir8); //42
    
    }else{
        agregar_buffer_uint32(buffer,32);
        agregar_buffer_uint32(buffer, valorAEscribir32);
    }

    agregar_buffer_uint32(buffer, pid_de_cde_ejecutando);
    agregar_buffer_uint32(buffer, numPagina);
    enviar_buffer(buffer, fd_memoria);
    destruir_buffer(buffer);

    // Memoria escribe lo que le llego en la df
    mensajeCpuMemoria codigoMemoria = recibir_codOp(fd_memoria);

    if(codigoMemoria == MOV_OUT_OK){

        if(es_uint8(registro)){
            log_info(logger_cpu,"PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %d", pid_de_cde_ejecutando, dirFisica, valorAEscribir8);
        }else {
            log_info(logger_cpu,"PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %d", pid_de_cde_ejecutando, dirFisica, valorAEscribir32);
        }
    }
}

bool es_uint8(char* registro){
    return (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0 || strcmp(registro, "CX") == 0 || strcmp(registro, "DX") == 0);
}

void ejecutar_io_stdin_read(char* interfaz, char* dir_logica, char* registro_tamaño){
    
    uint32_t direccion_Logica;
    uint32_t direccion_Fisica;
    uint32_t tamaño_registro;
    enviar_codOp(fd_kernel_int, INTERRUPT);

    t_buffer* buffer_a_enviar = crear_buffer();
    
    if (es_uint8(registro_tamaño)){
        tamaño_registro = (uint32_t)buscar_valor_registro8(registro_tamaño);
    } else {
        tamaño_registro = buscar_valor_registro32(registro_tamaño);
    }
    agregar_buffer_uint32(buffer_a_enviar, tamaño_registro);


    if (es_uint8(dir_logica)){
        direccion_Logica = (uint32_t)buscar_valor_registro8(dir_logica);
    } else {
        direccion_Logica  = buscar_valor_registro32(dir_logica);
    }
    direccion_Fisica = calcular_direccion_fisica(direccion_Logica);
    
    agregar_buffer_uint32(buffer_a_enviar,direccion_Fisica);

    agregar_buffer_string(buffer_a_enviar,interfaz);
    
    enviar_buffer(buffer_a_enviar,fd_kernel_int);
    destruir_buffer(buffer_a_enviar);
    interrupcion = 1;
}