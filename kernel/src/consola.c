#include "consola.h"


void iniciar_consola(){
	char* input;
	while(1){
		reset_terminal_mode();
		mostrar_opciones_consola(); // limpio el buffer (o algo asi xd)
		input = readline(">");
		if(strcmp(input, "EXIT") == 0) {
            free(input);
            break;
		}
		// add_history(input);
		atender_consola(input);
		free(input);
	}
}

void reset_terminal_mode() {
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ECHO | ICANON;
    tcsetattr(0, TCSANOW, &term);
}

void mostrar_opciones_consola(){
	printf("---MENU---\n");
	printf("1. EJECUTAR_SCRIPT\n"); //PATH => Archivo de comandos, se encuentra en maquina donde corra el kernel 1
	printf("2. INICIAR_PROCESO\n"); //PATH => Archivo pseudocodigo memoria 1
	printf("3. FINALIZAR_PROCESO\n"); //PID 1
	printf("4. INICIAR_PLANIFICACION\n"); //0
	printf("5. DETENER_PLANIFICACION\n"); //0
	printf("6. MULTIPROGRAMACION\n"); //VALOR 1
	printf("7. PROCESO_ESTADO\n"); // 0
	printf("8. EXIT\n"); //0
}


//INICIAR_PROCESO PATH
//INICIAR_PLANIFICACION

void atender_consola(char* input) {
    char** lista_mensaje = string_split(input, " ");
    int cant_par = string_array_size(lista_mensaje) - 1;
    if (lista_mensaje[1] == NULL) {
        lista_mensaje[1] = NULL;
    }
    t_codigo_operacion cod_operacion = get_codigo_operacion(lista_mensaje[0], cant_par);
    if (cod_operacion == EJECUTAR_SCRIPT) {
        t_list* lista_script = ejecutar_script(lista_mensaje[1]);
        string_array_destroy(lista_mensaje);
        
        t_link_element* actual = lista_script->head;
        while (actual != NULL) {
            t_script* comando = (t_script*)(actual->data);
            switch_comandos(comando->codigo_operacion, comando->par1);
            actual = actual->next;
        }
		//--CHECK--
        liberar_lista_script(lista_script);
    } else {
        switch_comandos(cod_operacion, lista_mensaje[1]);
    }
    string_array_destroy(lista_mensaje);
}


t_codigo_operacion get_codigo_operacion(char* comando, int cant_par){
	if(strcmp(comando, "EJECUTAR_SCRIPT") == 0){
		if(cant_par != 1){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: EJECUTAR_SCRIPT");
		return EJECUTAR_SCRIPT;
	}
	if(strcmp(comando, "INICIAR_PROCESO") == 0){
		if(cant_par != 1){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: INICIAR_PROCESO");
		return INICIAR_PROCESO;
	}
	else if(strcmp(comando, "FINALIZAR_PROCESO") == 0){
		if(cant_par != 1){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: FINALIZAR_PROCESO");
		return FINALIZAR_PROCESO;
	}
	else if(strcmp(comando, "INICIAR_PLANIFICACION") == 0){
		if(cant_par != 0){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: INICIAR_PLANIFICACION");
		return INICIAR_PLANIFICACION;
	}
	else if(strcmp(comando, "DETENER_PLANIFICACION") == 0){
		if(cant_par != 0){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: DETENER_PLANIFICACION");
		return DETENER_PLANIFICACION;
	}
	else if(strcmp(comando, "MULTIPROGRAMACION") == 0){
		if(cant_par != 1){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: MULTIPROGRAMACION");
		return MULTIPROGRAMACION;
	}
	else if(strcmp(comando, "PROCESO_ESTADO") == 0){
		if(cant_par != 0){return E_PARAMETROS;}
		log_info(logger_kernel, "La opcion elegida es: PROCESO_ESTADO");
		return PROCESO_ESTADO;
	}
	else if(strcmp(comando, "") == 0){
		log_info(logger_kernel, "No se recibio ningun valor.");
		return MENSAJE_VACIO;
	}
	else{
		log_info(logger_kernel, "El valor ingresado no es correcto.");
		return BASURA;
	}
}

void switch_comandos(uint8_t codOp, char* lista_mensaje){
	switch(codOp){
		case INICIAR_PROCESO:
			iniciar_proceso(lista_mensaje);
			free(lista_mensaje); 
			break;
		case FINALIZAR_PROCESO:
			finalizar_proceso(lista_mensaje);
			free(lista_mensaje); 
			break;
		case INICIAR_PLANIFICACION:
			log_info(logger_kernel, "INICIO DE PLANIFICACIÓN");
			iniciarPlanificacion();
			free(lista_mensaje);
			break;
		case DETENER_PLANIFICACION:
			log_info(logger_kernel, "PAUSA DE PLANIFICACIÓN");
			detenerPlanificacion();
			free(lista_mensaje);
			break;
		case MULTIPROGRAMACION:
			log_info(logger_kernel, "CAMBIO DE GRADO DE MULTIPROGRAMACION A: %s", lista_mensaje);
			cambiar_grado_multiprogramacion(lista_mensaje);
			free(lista_mensaje);
			break;
		case PROCESO_ESTADO:
			procesosPorEstado();
			free(lista_mensaje); 
			break;
		case E_PARAMETROS:
			log_info(logger_kernel, "Error en los parametros pasados");
			free(lista_mensaje);
			break;
		case MENSAJE_VACIO:
			log_info(logger_kernel, "Mensaje recibido vacio");
			free(lista_mensaje);
			break;
		case BASURA:
			log_info(logger_kernel, "Operacion desconocida");
			free(lista_mensaje);
			break;
		default:
			log_warning(logger_kernel,"Operacion desconocida (default)");
			free(lista_mensaje);
			break;
	}
}


//void script_destroy(t_script* script_a_destruir){
//	// free(script_a_destruir->codigo_operacion);
//	free(script_a_destruir->par1);
//	free(script_a_destruir);
//	return;
//}

void liberar_lista_script(t_list* lista) {
    for (int i = 0; i < list_size(lista); i++) {
        t_script* script = list_get(lista, i);
        if (script->par1 != NULL) {
            free(script->par1);
        }
        free(script);
    }
    list_destroy(lista);
}