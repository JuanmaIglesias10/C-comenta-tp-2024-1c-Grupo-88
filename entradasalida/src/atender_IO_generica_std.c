#include "atender_IO_generica_std.h"

// HILOS

void atender_memoria_IO_STDIN()
{
    while (1) {
        // por ahora no se usa
        sleep(10);
    }
}

void atender_memoria_IO_STDOUT()
{
    while (1) {
        // por ahora no se usa
        sleep(10);
    }
}

void atender_kernel_IO_GENERICA()
{
    while (1) {
            mensajeKernelIO cod_op = recibir_codOp(fd_kernel);
            switch (cod_op) {
                case SLEEP:
                    t_buffer* buffer_recibido = recibir_buffer(fd_kernel);
                    uint8_t unidadesDeTrabajo = leer_buffer_uint8(buffer_recibido);

                    usleep(unidadesDeTrabajo * config_IO_GENERICA.tiempo_unidad_trabajo);

                    destruir_buffer(buffer_recibido);
                    enviar_codOp(fd_kernel,SLEEP_OK);
                    break;
                default:
                    return;
            }
         }
}

void atender_kernel_IO_STDIN()
{
    while (1) {
            mensajeKernelIO cod_op = recibir_codOp(fd_kernel);
            switch (cod_op) {
                case STDIN_READ:
                    ejecutar_IO_STDIN_READ();
                    break;
                default:
                    return;
                }
        }
}

void atender_kernel_IO_STDOUT()
{
    while (1) {
            mensajeKernelIO cod_op = recibir_codOp(fd_kernel);
            switch (cod_op) {
                case STDOUT_WRITE:
                    ejecutar_IO_STDOUT_WRITE();
                    break;
                default:
                    return;
                }
        }
}

// FUNCIONES STDIN

void ejecutar_IO_STDIN_READ(){

	t_buffer* buffer_recibido = recibir_buffer(fd_kernel);
    uint32_t dir_fisica = leer_buffer_uint32(buffer_recibido); // la direccion donde se va a guardar el valor
	uint32_t tamMaximo = leer_buffer_uint32(buffer_recibido); // tamanio maximo del valor a leer
    uint32_t pid = leer_buffer_uint32(buffer_recibido); // pid del proceso que solicita la lectura

	destruir_buffer(buffer_recibido);
	int contador = 0;
    char* valor_ingresado;
    char* valor_truncado = malloc(tamMaximo + 1); 


	while(1){
		valor_ingresado = readline("Ingrese algo:");

		contador = strlen(valor_ingresado);

		if(contador < tamMaximo){
			log_warning(logger_IO, "Se corto el texto por superar el tamaño");
		} else if (contador > tamMaximo){
			log_warning(logger_IO, "El texto debe ser mas corto");
            strncpy(valor_truncado, valor_ingresado, tamMaximo);
            valor_truncado[tamMaximo] = '\0';
            break;
		} else {
            strncpy(valor_truncado, valor_ingresado, tamMaximo);
            valor_truncado[tamMaximo] = '\0';
            break;
        }

	}
    log_warning(logger_IO, "%s", valor_truncado);
    enviar_codOp(fd_memoria, IO_M_STDIN_READ_SOLICITUD);

	t_buffer* buffer = crear_buffer();
    agregar_buffer_uint32(buffer,dir_fisica);
    agregar_buffer_uint32(buffer,pid);
    agregar_buffer_string(buffer,valor_truncado);
    agregar_buffer_uint32(buffer,tamMaximo);
    enviar_buffer(buffer,fd_memoria); 
    destruir_buffer(buffer);

    mensajeIOMemoria cod_op = recibir_codOp(fd_memoria);

    if(cod_op == IO_M_STDIN_READ_OK) {
        enviar_codOp(fd_kernel,STDIN_READ_OK);
    } 
}
// FIN FUNCIONES STDIN

// FUNCIONES STDOUT

void ejecutar_IO_STDOUT_WRITE(){

	t_buffer* buffer_recibido = recibir_buffer(fd_kernel);
    uint32_t dir_fisica       = leer_buffer_uint32(buffer_recibido); 
	uint32_t tamaño_a_leer     = leer_buffer_uint32(buffer_recibido); 
    uint32_t pid              = leer_buffer_uint32(buffer_recibido);

	destruir_buffer(buffer_recibido);

    enviar_codOp(fd_memoria, IO_M_STDOUT_WRITE_SOLICITUD);

	t_buffer* buffer = crear_buffer();
    agregar_buffer_uint32(buffer,dir_fisica);
    agregar_buffer_uint32(buffer,pid);
    agregar_buffer_uint32(buffer,tamaño_a_leer);
    enviar_buffer(buffer,fd_memoria); 
    destruir_buffer(buffer);

    mensajeIOMemoria cod_op = recibir_codOp(fd_memoria);

    if(cod_op == IO_M_STDOUT_WRITE_OK) {
        t_buffer* buffer_string = recibir_buffer(fd_memoria);
        char* string_a_imprimir = leer_buffer_string(buffer_string);
        destruir_buffer(buffer_string);

        log_warning(logger_IO, "%s" , string_a_imprimir);

        enviar_codOp(fd_kernel,STDOUT_WRITE_OK);
    } 
}

// FIN FUNCIONES STDOUT