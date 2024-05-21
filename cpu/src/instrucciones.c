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

void ejecutar_set(char* registro, void* valor_recibido) {
     if (strcmp(registro, "PC") == 0) {
        registros_cpu->PC = *((uint32_t*)valor_recibido);
    }
    if (strcmp(registro, "AX") == 0) {
        registros_cpu->AX = *((uint8_t*)valor_recibido);
    }
    else if (strcmp(registro, "BX") == 0) {
        registros_cpu->BX = *((uint8_t*)valor_recibido);
    }
    else if (strcmp(registro, "CX") == 0) {
        registros_cpu->CX = *((uint8_t*)valor_recibido);
    }
    else if (strcmp(registro, "DX") == 0) {
        registros_cpu->DX = *((uint8_t*)valor_recibido);
    }
    else if (strcmp(registro, "EAX") == 0) {
        registros_cpu->EAX = *((uint32_t*)valor_recibido);
    }
    else if (strcmp(registro, "EBX") == 0) {
        registros_cpu->EBX = *((uint32_t*)valor_recibido);
    }
    else if (strcmp(registro, "ECX") == 0) {
        registros_cpu->ECX = *((uint32_t*)valor_recibido);
    }
    else if (strcmp(registro, "EDX") == 0) {
        registros_cpu->EDX = *((uint32_t*)valor_recibido);
    }
    else if (strcmp(registro, "SI") == 0) {
        registros_cpu->SI = *((uint32_t*)valor_recibido);
    }
    else if (strcmp(registro, "DI") == 0) {
        registros_cpu->DI = *((uint32_t*)valor_recibido);
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


void ejecutar_dormirIO(char* nombreInterfaz, uint8_t unidadesDeTiempo) {
    uint8_t numeroInterfaz = obtener_numero_interfaz(nombreInterfaz);
    if (numeroInterfaz == 1) {
        enviar_codOp(fd_kernel_int, INTERRUPT);
        t_buffer* buffer_a_enviar = crear_buffer();
        agregar_buffer_uint8(buffer_a_enviar,unidadesDeTiempo);
        agregar_buffer_uint8(buffer_a_enviar,numeroInterfaz);
        enviar_buffer(buffer_a_enviar,fd_kernel_int);
        destruir_buffer(buffer_a_enviar);
    }
    else {
        log_error(logger_cpu, "La interfaz %d no entiende esta instruccion", numeroInterfaz);
    }
    /* ACA HABRIA QUE INTERRUMPIR LA EJECUCION DEL PROCESO QUE EJECUTO ESTA INSTRUCCION,
       NO LO HAGO TODAVIA PORQUE NO TENEMOS MODELADOS COMO VAMOS A HACER LAS INTERRUPCIONES.
    */
}

uint8_t obtener_numero_interfaz(char* nombreInterfaz) {
    if(strcmp(nombreInterfaz, "Int1") == 0) { //Generica
        return 1;
    }
    if(strcmp(nombreInterfaz, "Int2") == 0) { //StdIn
        return 2;
    }
    if(strcmp(nombreInterfaz, "Int3") == 0) { //StdOut
        return 3;
    }
    if(strcmp(nombreInterfaz, "Int4") == 0) { //DialFS
        return 4;
    }
    else {
        return 100;
    }
}