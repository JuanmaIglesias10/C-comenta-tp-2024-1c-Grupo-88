#include "diccionario_IO.h"

// Logger Y Config
t_config_IO_GENERICA config_IO_GENERICA;
t_config_IO_STDIN config_IO_STDIN;
t_config_IO_STDOUT config_IO_STDOUT;
t_config_IO_DIALFS config_IO_DIALFS;
t_log* logger_IO;
t_config* config;
t_config* config_ip;

// Files Descriptors 
int fd_kernel;
int fd_memoria;

// FS
int tamanio_bitmap;
t_bitarray* bitarray;
t_list* lista_info_archivos;
char* path_archivo_bloques;
char* path_archivo_bitmap;