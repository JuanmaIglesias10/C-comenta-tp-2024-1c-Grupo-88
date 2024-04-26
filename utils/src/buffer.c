#include <buffer.h>

/*

// Crea un buffer vacío con offset 0
t_buffer* buffer_create() {
    t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = 0;
	buffer->stream = NULL;
	buffer->offset = 0;
	return buffer;
}

// Libera la memoria asociada al buffer
void buffer_destroy(t_buffer *buffer) {
    free(buffer->stream);
    free(buffer);
}

// Agrega un stream al buffer en la posición actual y avanza el offset
void buffer_add(t_buffer* buffer, void* data, uint32_t size) {
    buffer->stream = realloc(buffer->stream, buffer->size + size);
    memcpy(buffer->stream + buffer->size, data, size);
    buffer->size += size;
}

// Guarda size bytes del principio del buffer en la dirección data y avanza el offset
void buffer_read(t_buffer *buffer, void *data, uint32_t size) {
    uint32_t entero;
    memcpy(stream, buffer->stream, size);
}

// Agrega un uint32_t al buffer
void buffer_add_uint32(t_buffer *buffer, uint32_t data){
    buffer_add(buffer, &data, sizeof(uint32_t));
}

// Agrega un uint8_t al buffer
void buffer_add_uint8(t_buffer *buffer, uint8_t data) {
    buffer_add(buffer, &data, sizeof(uint8_t));
}

// UINT32
void buffer_write_uint32(t_buffer* buffer, uint32_t entero){
	buffer->stream = realloc(buffer->stream, buffer->size + sizeof(uint32_t));

	memcpy(buffer->stream + buffer->size, &entero, sizeof(uint32_t));
	buffer->size += sizeof(uint32_t);
}

uint32_t buffer_read_uint32(t_buffer* buffer){
	uint32_t entero;

	memcpy(&entero, buffer->stream + buffer->offset, sizeof(uint32_t));
	buffer->offset += sizeof(uint32_t);

	return entero;
}


// UINT8
void buffer_write_uint8(t_buffer* buffer, uint8_t entero){
	buffer->stream = realloc(buffer->stream, buffer->size + sizeof(uint8_t));

	memcpy(buffer->stream + buffer->size, &entero, sizeof(uint8_t));
	buffer->size += sizeof(uint8_t);
}

uint8_t buffer_read_uint8(t_buffer* buffer){
	uint8_t entero;

	memcpy(&entero, buffer->stream + buffer->offset, sizeof(uint8_t));
	buffer->offset += sizeof(uint8_t);

	return entero;
}

// STRING
void buffer_write_string(t_buffer* buffer, char* cadena){
	uint32_t tam = 0;

	while(cadena[tam] != NULL)
		tam++;

	buffer_write_uint32(buffer,tam);

	buffer->stream = realloc(buffer->stream, buffer->size + tam);

	memcpy(buffer->stream + buffer->size, cadena , tam);
	buffer->size += tam;
}

char* buffer_read_string(t_buffer* buffer, uint32_t* tam){
	(*tam) = buffer_read_uint32(buffer);
	char* cadena = malloc((*tam) + 1);
	

	memcpy(cadena, buffer->stream + buffer->offset, (*tam));
	buffer->offset += (*tam);

	*(cadena + (*tam)) = '\0';

	return cadena;
}

*/