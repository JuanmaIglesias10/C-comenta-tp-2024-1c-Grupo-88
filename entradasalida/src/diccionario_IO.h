#ifndef DICCIONARIO_IO_H_
#define DICCIONARIO_IO_H_

#include "utils.h"
#include "commons/bitarray.h"

typedef struct{
    char* tipo_interfaz;
    int tiempo_unidad_trabajo;
    char* ip_kernel;
    int puerto_kernel;
} t_config_IO_GENERICA;

typedef struct{
    char* tipo_interfaz;
    char* ip_kernel;
    int puerto_kernel;
    char* ip_memoria;
    int puerto_memoria;
} t_config_IO_STDIN;

typedef struct{
    char* tipo_interfaz;
    char* ip_kernel;
    int puerto_kernel;
    char* ip_memoria;
    int puerto_memoria;
} t_config_IO_STDOUT;

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
} t_config_IO_DIALFS;

typedef struct{
    char* nombre_archivo;
    t_config* config_archivo;
} t_info_archivo;

// Logger Y Config
extern t_config_IO_GENERICA config_IO_GENERICA;
extern t_config_IO_STDIN config_IO_STDIN;
extern t_config_IO_STDOUT config_IO_STDOUT;
extern t_config_IO_DIALFS config_IO_DIALFS;
extern t_log* logger_IO;
extern t_config* config;
extern t_config* config_ip;

// Files Descriptors 
extern int fd_kernel;
extern int fd_memoria;

// FS
extern int tamanio_bitmap;
extern t_bitarray* bitarray;
extern t_list* lista_info_archivos;
extern char* path_archivo_bloques;
extern char* path_archivo_bitmap;

#endif /* DICCIONARIO_IO_H_ */