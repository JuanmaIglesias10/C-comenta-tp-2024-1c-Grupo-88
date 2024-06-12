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
        //         log_info(logger_IO, "Se desconectó MEMORIA");
        //         return;
        //     }
    }
return NULL;
}

void atender_kernel()
{
     while (1) {
             mensajeKernelIO cod_op = recibir_codOp(fd_kernel);
                    log_info(logger_IO,"%d",cod_op);
                    log_info(logger_IO,"xd");
             switch (cod_op) {
                 case SLEEP:
                    //  exec_IO_Generica();
                     log_info(logger_IO,"1");
                    t_buffer* buffer_recibido = recibir_buffer(fd_kernel);
                    uint8_t unidadesDeTiempo = leer_buffer_uint8(buffer_recibido);
                    usleep(unidadesDeTiempo * 1000);
                    log_info(logger_IO,"2");
                    destruir_buffer(buffer_recibido);
                    enviar_codOp(fd_kernel,SLEEP_OK);
                    log_info(logger_IO,"3");
                    break;
                 default:
                     log_info(logger_IO, "Se desconectó KERNEL");
                     return;
                 }
         }
}