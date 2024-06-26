#ifndef ATENDER_IO_DIALFS_H_
#define ATENDER_IO_DIALFS_H_

#include "IO.h"
#include "diccionario_IO.h"

#include "commons/bitarray.h"

void atender_memoria_IO_DIALFS();
void atender_kernel_IO_DIALFS();

void inicializar_archivos_FS();
void inicializar_bitmap();
char* bool_to_string(bool value);

#endif /* ATENDER_IO_DIALFS_H_ */