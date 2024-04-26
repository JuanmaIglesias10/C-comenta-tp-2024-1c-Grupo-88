#ifndef PROTOCOLO_H
#define PROTOCOLO_H

//#include <utils.h>
#include <commons/collections/list.h> // SI
#include <commons/log.h> // SI
#include <sys/socket.h> // SI
#include <stdlib.h> // malloc, free, realloc -- SI
#include <string.h> // memcpy, strlen - SI
#include <unistd.h> // close,
#include <netdb.h>


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


void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(op_code);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
t_list* recibir_paquete(int);
uint8_t recibir_codOp(int);

// ----------------------- BUFFER -------------------------
t_buffer* recibir_buffer(int socket_cliente);

// Crea un buffer vacío con offset 0
t_buffer* crear_buffer();

// Libera la memoria asociada al buffer
void destruir_buffer(t_buffer *buffer);

// Agrega un stream al buffer en la posición actual y avanza el offset
void agregar_a_buffer(t_buffer *buffer, void *data, uint32_t size);

// Guarda size bytes del principio del buffer en la dirección data y avanza el offset
void* leer_buffer(t_buffer *buffer, uint32_t size);

// ESPECIFICOS

// Agrega un uint32_t al buffer
void agregar_buffer_uint32(t_buffer* buffer, uint32_t data);

// Lee un uint32_t del buffer y avanza el offsets
uint32_t leer_buffer_uint32(t_buffer* buffer);

// Agrega un uint8_t al buffer
void agregar_buffer_uint8(t_buffer* buffer, uint8_t data);

// Lee un uint8_t del buffer y avanza el offset
uint8_t leer_buffer_uint8(t_buffer* buffer);

// Agrega string al buffer con un uint32_t adelante indicando su longitud
// calcula la longitud del string en vez de recibirla por parametro
void agregar_buffer_string(t_buffer* buffer, char* string);

// Lee un string y su longitud del buffer y avanza el offset
char* leer_buffer_string(t_buffer* buffer);


#endif /* PROTOCOLO_H_ */