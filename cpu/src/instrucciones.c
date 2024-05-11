#include "instrucciones.h"


char* obtener_nombre_instruccion(t_instruccion* instruccion){
    switch(instruccion->codigo){
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


