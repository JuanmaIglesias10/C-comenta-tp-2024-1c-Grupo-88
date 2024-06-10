#include "diccionario_cpu.h"
#include "utils.h"


char* obtener_nombre_instruccion(t_instruccion* instruccion);

// FUNCIONALIDAD DE LAS INSTRUCCIONES
void ejecutar_sum(char* reg_dest, char* reg_origen);
void ejecutar_set8(char* registro, char* char_valor_recibido);
void ejecutar_set32(char* registro, char* char_valor_recibido);
void ejecutar_sub(char* reg_dest, char* reg_origen);
uint8_t buscar_valor_registro8(void* registro);
uint32_t buscar_valor_registro32(void* registro);
void* buscar_valor_registro(void* registro);
void ejecutar_jnz(void* registro, char* char_nro_instruccion);
// void ejecutar_dormirIO(char* nombreInterfaz, uint32_t unidadesDeTiempo);
void ejecutar_dormirIO(char* nombreInterfaz, char* charUnidadesDeTiempo);
void ejecutar_exit();
uint8_t obtener_numero_interfaz(char* nombreInterfaz);