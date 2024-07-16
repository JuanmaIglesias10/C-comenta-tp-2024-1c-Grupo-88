#ifndef ATENDER_IO_DIALFS_H_
#define ATENDER_IO_DIALFS_H_

#include "IO.h"
#include "diccionario_IO.h"

#include "commons/config.h"

#include "commons/bitarray.h"
#include "commons/memory.h"
#include "dirent.h"
#include "math.h" // para floor() y ceil()
#include <sys/mman.h>

void atender_memoria_IO_DIALFS();
void atender_kernel_IO_DIALFS();

void testeo_FS();

void crear_bitarray(void);
bool archivos_base_existen(void);
void crear_archivos_base(void);
void sincronizar_bitarray(void);

void mostrar_bitmap(void);
void mostrar_bitarray(t_bitarray* un_bitarray);
char* bool_to_string(bool value);
void bitarray_clean(t_bitarray* un_bitarray);
bool existe_archivo(char *nombre_archivo);

void fs_crear_archivo(char* nombre_archivo);
int encontrarPrimerBloqueLibre(void);

t_config* obtener_metadata(char* nombre_archivo);
uint32_t tamanio_archivo(char* nombre_archivo);
uint32_t bloque_inicial_archivo(char* nombre_archivo);

void fs_truncar_archivo(char* nombre_archivo, uint32_t nuevo_tamanio);

uint32_t contar_bloques_siguientes_libres(uint32_t ultimo_bloque_actual);
uint32_t hay_espacio_contiguo_libre_en_algun_lado (uint32_t cantidad_bloques_nueva);



#endif /* ATENDER_IO_DIALFS_H_ */