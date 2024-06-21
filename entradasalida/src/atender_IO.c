#include "atender_IO.h"

void* atender_memoria()
{
while (1) {
        // uint8_t cod_op = recibir_codOp(fd_memoria);
        // switch (cod_op) {
        //     case MENSAJE:
        //         //
        //         break;
        //     case PAQUETE:
        //         //
        //         break;
        //     default:
        //         return;
        //     }
    }
return NULL;
}

void atender_kernel()
{
     while (1) {
             mensajeKernelIO cod_op = recibir_codOp(fd_kernel);
             switch (cod_op) {
                 case SLEEP:
                    t_buffer* buffer_recibido = recibir_buffer(fd_kernel);
                    uint8_t unidadesDeTiempo = leer_buffer_uint8(buffer_recibido);
                    usleep(unidadesDeTiempo * 1000);
                    destruir_buffer(buffer_recibido);
                    enviar_codOp(fd_kernel,SLEEP_OK);
                    break;
                 case STDIN_READ:
                    exec_IO_STDIN_READ();
                    usleep(unidadesDeTiempo * 1000);
                    enviar_codOp(fd_kernel,STDIN_READ_OK);
                    break;

                 default:
                     return;
                 }
         }
}