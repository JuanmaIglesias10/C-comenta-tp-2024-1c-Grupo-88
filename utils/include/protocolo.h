#ifndef PROTOCOLO_H
#define PROTOCOLO_H

//#include <utils.h>
#include <commons/collections/list.h> // SI
#include <commons/log.h> // SI
#include <sys/socket.h> // SI
#include <stdlib.h> // malloc, free, realloc -- SI
#include <string.h> // memcpy, strlen - SI
#include <unistd.h> // close, 

#include <buffer.h> // SI

void duplicarAltura (t_arbol arbol);

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

#endif /* PROTOCOLO_H_ */