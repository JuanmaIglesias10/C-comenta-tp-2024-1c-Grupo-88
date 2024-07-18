#ifndef MANEJO_IO_H_
#define MANEJO_IO_H_

#include "kernel.h"
#include "diccionario_kernel.h"

void io_gen_sleep(char* interfaz, char* char_unidadesDeTrabajo);
void* io_stdin_read(void* arg);
void io_stdout_write(char* interfaz, char* char_direccion_fisica, char* char_tamanio);

void io_fs_create(char* interfaz, char* nombreArchivo);
void io_fs_delete(char* interfaz, char* nombreArchivo);
void* io_fs_truncate(void* arg); // 3 parametros: interfaz, nombre, tamanio
void* io_fs_write(void* arg); // 3 parametros: interfaz, nombre, dir_fisica, tamanio, valor puntero
void* io_fs_read(void* arg); // 3 parametros: interfaz, nombre, dir_fisica, tamanio, valor puntero

#endif /* MANEJO_IO_H_ */