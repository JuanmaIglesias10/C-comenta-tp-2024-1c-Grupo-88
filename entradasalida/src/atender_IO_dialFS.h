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
void actualizar_bitmap(void);
void leer_bitmap(void);
void mostrar_bitmap(void);
void mostrar_bitarray(t_bitarray* un_bitarray);
char* bool_to_string(bool value);
void bitarray_clean(t_bitarray* un_bitarray);
bool existe_archivo(char *nombre_archivo);
void leer_info_archivos(void);
void mostrar_info_archivos(void);
t_info_archivo* obtener_info_archivo(char* nombre_archivo_buscado);

void fs_crear_archivo(char* nombre_archivo);
int encontrarPrimerBloqueLibre(void);

void fs_truncar_archivo(char* nombre_archivo, uint32_t nuevo_tamanio);
uint32_t contar_bloques_siguientes_libres(uint32_t ultimo_bloque_actual);
void compactar_FS(void);
uint32_t buscar_primer_lugar_libre(uint32_t cantidad_bloques_necesaria);

void fs_eliminar_archivo(char* nombre_archivo);

char* fs_leer_archivo(char* nombre_archivo, uint32_t puntero_archivo, uint32_t tamanio_a_leer);
void fs_escribir_archivo(char* nombre_archivo, uint32_t puntero_archivo, uint32_t tamanio_a_escribir, char* info_a_escribir);


#endif /* ATENDER_IO_DIALFS_H_ */