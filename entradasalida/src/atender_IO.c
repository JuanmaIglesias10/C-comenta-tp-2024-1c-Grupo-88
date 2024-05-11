#include "atender_IO.h"

void* atender_memoria()
{
while (1) {
        uint8_t cod_op = recibir_codOp(fd_memoria);
        switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_IO, "Se desconectó MEMORIA");
                break;
            }
    }
return NULL;
}

void* atender_kernel()
{
while (1) {
        uint8_t cod_op = recibir_codOp(fd_kernel);
        switch (cod_op) {
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            default:
                log_info(logger_IO, "Se desconectó KERNEL");
                break;
            }
    }
return NULL;
}