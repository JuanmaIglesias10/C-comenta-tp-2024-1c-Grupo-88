#ifndef ATENDER_IO_DIALFS_H_
#define ATENDER_IO_DIALFS_H_

#include "IO.h"
#include "diccionario_IO.h"

#include "commons/bitarray.h"
#include "commons/memory.h"
#include "dirent.h"

void atender_memoria_IO_DIALFS();
void atender_kernel_IO_DIALFS();

void testeo_FS();

void crear_bitarray(void);
int archivos_base_existen(void);
void crear_archivos_base(void);
void sincronizar_bitarray(void);

void mostrar_bitmap(void);
void mostrar_bitarray(t_bitarray* unBitarray);
char* bool_to_string(bool value);
void bitarray_clean(t_bitarray* bitarray);
int file_exists_in_directory(const char *directory, const char *nombre_archivo);

void fs_crear_archivo(char* nombreArchivo);

int asignarPrimerBloqueLibre(void);

#endif /* ATENDER_IO_DIALFS_H_ */