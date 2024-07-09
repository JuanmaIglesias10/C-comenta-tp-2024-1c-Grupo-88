#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "utils.h"
#include "atender_memoria.h"
#include "diccionario_memoria.h"
#include "commons/temporal.h"
#include "math.h"

// Function declarations
void inicializar_memoria();
void inicializar_config();
void inicializar_variables();
void inicializar_conexiones();
void enviar_tamanio_pagina();
void iniciar_proceso();

t_proceso* crear_proceso(t_list* listaInstrucciones, uint32_t pid);

t_list* obtener_instrucciones(char* rutaArchivo);

t_codigo_instruccion obtener_codigo_instruccion(char* charParametro);

void obtener_parametros_instruccion(int numParametro, t_instruccion* instrucciones, char* parametros);

void instrucciones_destroy(t_instruccion* instrucciones_a_destruir);

void enviar_instruccion();

void ejecutar_MOV_OUT();

void ejecutar_MOV_IN();

void resize();

void devolver_nro_marco();

t_proceso* buscarProcesoPorPid(uint32_t pid);

bool hay_marcos_libres(int cantMarcosNecesitada);

t_pagina* buscarPaginaPorNroYPid(t_proceso* proceso , uint32_t nroPag);

t_pagina* crear_pagina(uint32_t nroPag, uint32_t nroMarco, void* dirreccionInicio, uint32_t pid);

void colocar_pagina_en_marco(uint32_t pid, uint32_t nroPagina);

uint32_t obtener_marco_libre();

uint32_t obtener_pagina_libre();

void escribir_pagina(uint32_t posEnMemoria, void* pagina);

void liberarPaginasDeUnProcesoResize(t_proceso* procesoAReducir, uint32_t nuevoTamaño);

void vaciar_marco(uint32_t nroMarco);

void escribirValorEnMemoria(uint32_t valor, size_t dirFisica, int cantBytes, t_proceso* proceso);

void escribirValorEnMemoriaString(char* valor, size_t dirFisica, int tamString, t_proceso* proceso);

char* leerValorEnMemoriaString(size_t dirFisica, int tamString, t_proceso* proceso);

uint32_t leerValorEnMemoria(size_t dirFisica, int cantBytes, t_proceso* proceso);

void imprimir_memoria();

void escribir_stdin_read(int fd_IO);

void leer_stdout_write(int fd_IO);

void ejecutar_copy_string();
#endif /* MEMORIA_H_ */

