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
                    // TODO
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

	destruir_buffer(buffer_recibido);
	int contador = 0;
    char* valor_ingresado;
	while(contador != tamMaximo){
		valor_ingresado = readline("Ingrese algo:");

		contador = strlen(valor_ingresado);

		if(contador < tamMaximo){
			log_warning(logger_IO, "El texto debe ser mas largo"); // Modificarlo en base a lo que dijeron en el soporte
		} else if (contador > tamMaximo){                          // O lo dejamos asi como vimos en un issue?
			log_warning(logger_IO, "El texto debe ser mas corto");
		}

	}

    enviar_codOp(fd_memoria, STDIN_READ);

	t_buffer* buffer = crear_buffer();
    agregar_buffer_uint32(buffer,dir_fisica);
    agregar_buffer_uint32(buffer,tamMaximo);
    agregar_buffer_string(buffer,valor_ingresado);
	//if(tam_reg8 != 0){                   ESTO DEL TAMAÑO LO VOLAMOS, NO?  
	//	agregar_buffer_uint32(buffer,8);    
	//} else{                                 
	//	agregar_buffer_uint32(buffer,32);
	//}
    enviar_buffer(buffer,fd_memoria); 
    destruir_buffer(buffer);
    
    //TODO: Falta la parte de memoria, recibir el buffer y hacer lo que tiene que hacer

    mensajeIOMemoria cod_op = recibir_codOp(fd_memoria);

    if(cod_op == READ_OK) enviar_codOp(fd_kernel,STDIN_READ_OK); // si esta todo ok desde memoria, enviamos el ok a kernel.
}
// FIN FUNCIONES STDIN

// FUNCIONES STDOUT

void ejecutar_IO_STDOUT_WRITE(){

}

// FIN FUNCIONES STDOUT