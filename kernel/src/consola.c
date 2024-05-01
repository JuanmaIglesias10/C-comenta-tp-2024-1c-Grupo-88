#include "consola.h"


void inciar_consola(){
	char* input;
	while(1){
		mostrar_opciones_consola();
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


void atender_consola(char* input){ 
	char** lista_mensaje = string_split(input, " ");
	int cant_par = string_array_size(lista_mensaje) - 1;
	t_codigo_operacion cod_operacion = get_codigo_operacion(lista_mensaje[0], cant_par);

	switch(cod_operacion){
		case EJECUTAR_SCRIPT:
			// ejecutar_script(lista_mensaje[1]);
			string_array_destroy(lista_mensaje); 
            break;
    	case INICIAR_PROCESO:
			iniciarProceso(lista_mensaje[1]);
			string_array_destroy(lista_mensaje); 
            break;
    	case FINALIZAR_PROCESO:
			// finalizarProceso(lista_mensaje[1]);
			//detectarDeadlock();
			string_array_destroy(lista_mensaje); 
    		break;
    	case INICIAR_PLANIFICACION:
    		log_info(logger_kernel, "INICIO DE PLANIFICACIÓN");
			// iniciarPlanificacion();
			string_array_destroy(lista_mensaje);
    		break;
    	case DETENER_PLANIFICACION:
    		log_info(logger_kernel, "PAUSA DE PLANIFICACIÓN");
			// detenerPlanificacion();
			string_array_destroy(lista_mensaje);
    		break;
    	case MULTIPROGRAMACION:
			// cambiar_grado_multiprogramacion(lista_mensaje[1]);
			string_array_destroy(lista_mensaje);
    		break;
    	case PROCESO_ESTADO:
			// procesosPorEstado();
			string_array_destroy(lista_mensaje); 
    		break;
		case E_PARAMETROS:
			log_info(logger_kernel, "Error en los parametros pasados");
			string_array_destroy(lista_mensaje);
			break;
		case MENSAJE_VACIO:
			log_info(logger_kernel, "Mensaje recibido vacio");
			string_array_destroy(lista_mensaje);
			break;
		case BASURA:
			log_info(logger_kernel, "Operacion desconocida");
			string_array_destroy(lista_mensaje);
			break;
		default:
			log_warning(logger_kernel,"Operacion desconocida (default)");
			string_array_destroy(lista_mensaje);
			break;
    }
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
