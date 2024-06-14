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

void ejecutar_set8(char* registro, char* char_valor_recibido) {
    uint8_t valor_recibido = atoi(char_valor_recibido);
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

void ejecutar_set32(char* registro, char* char_valor_recibido){
    uint32_t valor_recibido = atoi(char_valor_recibido);
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

void ejecutar_io_gen_sleep(char* nombreInterfaz, char* charUnidadesDeTiempo) {
    uint32_t unidadesDeTiempo = atoi(charUnidadesDeTiempo);
    // log_info(logger_cpu, unidadesDeTiempo);
    enviar_codOp(fd_kernel_int, INTERRUPT);
    t_buffer* buffer_a_enviar = crear_buffer();
    agregar_buffer_uint8(buffer_a_enviar,unidadesDeTiempo);
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

void ejecutar_mov_in(char* registro, char* charDirLogica, t_cde* cde){
    uint32_t dirLogica = atoi(charDirLogica);
    uint32_t dirFisica = calcular_direccion_fisica(dirLogica, cde);
    uint32_t numPagina = obtener_numero_pagina(dirLogica);
    //REVISAR
    // if(interruption){ // significa que hubo page fault
    //     return;
    // }

    enviar_codOp(fd_memoria, MOV_IN_SOLICITUD);
    
    t_buffer* buffer = crear_buffer();
    agregar_buffer_uint32(buffer, dirFisica);
    agregar_buffer_uint32(buffer, cde->pid);
    agregar_buffer_uint32(buffer, numPagina);
    enviar_buffer(buffer, fd_memoria);
    destruir_buffer(buffer);
    
    mensajeCpuMemoria codigoMemoria = recibir_codOp(fd_memoria);

    if(codigoMemoria == MOV_IN_OK){
        buffer = recibir_buffer(fd_memoria);
        char* valorLeido = leer_buffer_string(buffer); //RECIBIR COMO STRING EN MEMORIA.C
        ejecutar_set32(registro, valorLeido);

        destruir_buffer(buffer);

        log_info(logger_cpu,"PID: %d - Acción: LEER - Dirección Física: %d - Valor: %s", cde->pid, dirFisica, valorLeido);
    }
}