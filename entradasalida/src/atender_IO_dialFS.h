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

void inicializar_archivos_FS();

char* bool_to_string(bool value);

void bitarray_clean(t_bitarray* bitarray);
void mostrar_bitarray(t_bitarray* bitarray);

int file_exists_in_directory(const char *directory, const char *nombre_archivo);

#endif /* ATENDER_IO_DIALFS_H_ */