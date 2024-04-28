#ifndef DICCIONARIO_IO_H_
#define DICCIONARIO_IO_H_

#include <commons/log.h>
#include <commons/config.h> 

#include "utils.h"

//cine
typedef struct{
    char* tipo_interfaz;
    int tiempo_unidad_trabajo;
    char* ip_kernel;
    int puerto_kernel;
    char* ip_memoria;
    int puerto_memoria;
    char* path_base_dialfs;
    int block_size;
    int block_count;
    int retraso_compactacion;
}t_config_IO;

// Logger Y Config
extern t_config_IO config_IO;
extern t_log* logger_IO;
extern t_config* config;

// Files Descriptors 
extern int fd_kernel;
extern int fd_memoria;


#endif /* DICCIONARIO_IO_H_ */