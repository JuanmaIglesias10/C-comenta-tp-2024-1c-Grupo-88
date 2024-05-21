#include "diccionario_cpu.h"
#include "utils.h"


char* obtener_nombre_instruccion(t_instruccion* instruccion);

// FUNCIONALIDAD DE LAS INSTRUCCIONES
void ejecutar_set(char* registro, void* valor_recibido);
void ejecutar_sum(char* reg_dest, char* reg_origen);
uint8_t  buscar_valor_registro(void* registro);
void ejecutar_sub(char* reg_dest, char* reg_origen);
void ejecutar_jnz(void* registro, uint32_t nro_instruccion, t_cde* cde);
void ejecutar_dormirIO(int nombreInterfaz, int unidadesDeTiempo);
