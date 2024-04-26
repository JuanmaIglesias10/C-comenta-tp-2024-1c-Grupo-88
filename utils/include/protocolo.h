#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <utils.h>

typedef enum
{
	MENSAJE,
	PAQUETE
} op_code;

typedef struct
{
	int32_t size;
	int32_t offset;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_mensaje(char* mensaje, int socket_cliente);
void crear_buffer(t_paquete* paquete);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
t_list* recibir_paquete(int);
void recibir_mensaje(int, t_log*);
int recibir_operacion(int);
void* recibir_buffer(int* size, int socket_cliente);

#endif