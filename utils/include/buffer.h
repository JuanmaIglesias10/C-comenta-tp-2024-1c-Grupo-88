#ifndef BUFFER_H
#define BUFFER_H

//#include <utils.h>
#include <netdb.h> // SI -- uint32_t, uint8_t
#include <stdlib.h> // malloc, free, realloc -- SI
#include <string.h> // memcpy, strlen - SI

typedef struct {
    int altura;
} t_arbol;

typedef struct
{
	uint32_t size;
	uint32_t offset;
	void* stream;
} t_buffer;

typedef enum
{
	MENSAJE,
	PAQUETE
} op_code;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

// Crea un buffer vacío con offset 0
t_buffer* buffer_create();

// Libera la memoria asociada al buffer
void buffer_destroy(t_buffer *buffer);

// Agrega un stream al buffer en la posición actual y avanza el offset
void buffer_add(t_buffer *buffer, void *data, uint32_t size);

// Guarda size bytes del principio del buffer en la dirección data y avanza el offset
void buffer_read(t_buffer *buffer, void *data, uint32_t size);

// ESPECIFICOS

// Agrega un uint32_t al buffer
void buffer_add_uint32(t_buffer* buffer, uint32_t data);

// Lee un uint32_t del buffer y avanza el offsets
uint32_t buffer_read_uint32(t_buffer* buffer);

// Agrega un uint8_t al buffer
void buffer_add_uint8(t_buffer* buffer, uint8_t data);

// Lee un uint8_t del buffer y avanza el offset
uint8_t buffer_read_uint8(t_buffer* buffer);

// Agrega string al buffer con un uint32_t adelante indicando su longitud
void buffer_add_string_1(t_buffer* buffer, uint32_t length, char* string);

// Agrega string al buffer con un uint32_t adelante indicando su longitud
// calcula la longitud del string en vez de recibirla por parametro
void buffer_add_string_2(t_buffer* buffer, char* string);

// Lee un string y su longitud del buffer y avanza el offset
char* buffer_read_string(t_buffer* buffer, uint32_t* length);

// Lee un string y su longitud del buffer y avanza el offset
char *buffer_read_string(t_buffer *buffer, uint32_t *length);

#endif /* BUFFER_H_ */