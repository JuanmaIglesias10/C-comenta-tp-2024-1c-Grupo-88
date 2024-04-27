#include "protocolo.h"

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
} // TODO: revisar

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
} // TODO: revisar

t_paquete* crear_paquete(op_code codigo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
 	paquete->buffer =  crear_buffer();
	return paquete;
} // TODO: revisar

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio) // stream_add
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
} // TODO: revisar

void enviar_paquete(t_paquete* paquete, int socket_cliente) 
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
} // TODO: revisar

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
} // TODO: revisar

t_list* deserializar_lista_strings(t_buffer* buffer){
	t_list* lista_strings = list_create();
	uint32_t string_size;

	while(buffer->offset < buffer->size)
	{
		string_size = leer_buffer_uint32(buffer);
		char* string = malloc(string_size);
		string = leer_buffer_string(buffer);
		list_add(lista_strings, string);
	}
	free(buffer);
	return lista_strings;
} // OK

t_list* deserializar_lista(t_buffer* buffer){
	t_list* lista = list_create();
	uint32_t elem_size;

	while(buffer->offset < buffer->size)
	{
		elem_size = leer_buffer_uint32(buffer);
		void* elem = malloc(elem_size);
		elem = leer_buffer(buffer, elem_size);
		list_add(lista, elem);
	}
	free(buffer);
	return lista;
} // OK

uint8_t recibir_codOp(int socket_conexion)
{
	uint8_t cod_op;
	if(recv(socket_conexion, &cod_op, sizeof(uint8_t), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_conexion);
		exit(1);
	}
} // TODO: ver si esta ok lo del exit

t_buffer* recibir_buffer(int socket_conexion){
    t_buffer* buffer = crear_buffer();
    // Recibo el tamanio del buffer y reservo espacio en memoria

    recv(socket_conexion, &(buffer -> size), sizeof(uint32_t), MSG_WAITALL);

    if(buffer->size != 0){
        buffer -> stream = malloc(buffer -> size);
        // Recibo stream del buffer
        recv(socket_conexion, buffer -> stream, buffer -> size, MSG_WAITALL);
    }
    return buffer;
} // OK


void enviar_buffer(t_buffer* buffer, int socket_conexion){

    // Enviamos el tamanio del buffer
    send(socket_conexion, &(buffer->size), sizeof(uint32_t), 0);

    if (buffer->size != 0){
        // Enviamos el stream del buffer
        send(socket_conexion, buffer->stream, buffer->size, 0);
    }
} // TODO: no se podria hacer 1 solo send y listo?


// ---------------- BUFFER -------------------
// Crea un buffer vacío con offset 0
t_buffer* crear_buffer() {
    t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = 0;
	buffer->stream = NULL;
	buffer->offset = 0;
	return buffer;
} // OK

// Libera la memoria asociada al buffer
void destruir_buffer(t_buffer* buffer) {
    free(buffer->stream);
    free(buffer);
} // OK

// Agrega un stream al final del buffer
void agregar_a_buffer(t_buffer* buffer, void* data, uint32_t size) {
	if (buffer == NULL || data == NULL || size == 0) return;
    buffer->stream = realloc(buffer->stream, buffer->size + size);
    memcpy(buffer->stream + buffer->size, data, size);
    buffer->size += size;
} // OK

// Guarda size bytes del principio del buffer en la dirección data y avanza el offset
void* leer_buffer(t_buffer* buffer, uint32_t size) {
	if (buffer == NULL || size == 0) return NULL;
	void* data = malloc(size);
    memcpy(data, buffer->stream + buffer->offset, size);
    buffer->offset += size;
	return data;
} // OK

// Agrega un uint32_t al buffer
void agregar_buffer_uint32(t_buffer* buffer, uint32_t data){
    agregar_a_buffer(buffer, &data, sizeof(uint32_t));
} // OK

// Lee un uint32_t del buffer y avanza el offset
uint32_t leer_buffer_uint32(t_buffer* buffer){
	uint32_t* data = leer_buffer(buffer, sizeof(uint32_t));
    return *data;
} // OK

// Agrega un uint8_t al buffer
void agregar_buffer_uint8(t_buffer* buffer, uint8_t data) {
    agregar_a_buffer(buffer, &data, sizeof(uint8_t));
} // OK

uint8_t leer_buffer_uint8(t_buffer* buffer) {
    uint8_t* data = leer_buffer(buffer, sizeof(uint8_t));
    return *data;
} // OK

// Agrega string al buffer con un uint32_t adelante indicando su longitud
void agregar_buffer_string(t_buffer* buffer, char* string) {
	uint32_t length = strlen(string);
    agregar_buffer_uint32(buffer, length);
    agregar_a_buffer(buffer, string, length); 
} // OK

// Lee un string y su longitud del buffer y avanza el offset
char* leer_buffer_string(t_buffer* buffer) {
    uint32_t length = leer_buffer_uint32(buffer); 
    char* string = malloc(length + 1); 
    if (string == NULL) return NULL;
    string = leer_buffer(buffer, length);
    string[length] = '\0'; // Null-terminate the string
    return string;
} // OK