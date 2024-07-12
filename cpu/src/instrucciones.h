#include "diccionario_cpu.h"
#include "utils.h"
#include "mmu.h"


char* obtener_nombre_instruccion(t_instruccion* instruccion);

// FUNCIONALIDAD DE LAS INSTRUCCIONES
void ejecutar_sum(char* reg_dest, char* reg_origen);
void ejecutar_set8(char* registro, uint8_t valor_recibido);
void ejecutar_set32(char* registro, uint32_t valor_recibido);
void ejecutar_sub(char* reg_dest, char* reg_origen);
uint8_t buscar_valor_registro8(void* registro);
uint32_t buscar_valor_registro32(void* registro);
void* buscar_valor_registro(void* registro);
void ejecutar_jnz(void* registro, char* char_nro_instruccion);
void ejecutar_io_gen_sleep(char* nombreInterfaz, char* charUnidadesDeTiempo);
void ejecutar_wait(char* recurso);
void ejecutar_signal(char* recurso);
int ejecutar_resize(char* tamanio);
void ejecutar_exit();
uint8_t obtener_numero_interfaz(char* nombreInterfaz);
void ejecutar_mov_in(char* registro, char* charDirLogica);
void ejecutar_mov_out(char* registroDirLogica, char* registro);
bool es_uint8(char* registro);
void ejecutar_io_stdin_read(char* interfaz, char** dir_logica, char** registro_tamanio);
void ejecutar_io_stdout_write(char* interfaz, char** dir_logica, char** registro_tamanio);
void ejecutar_copy_string(char* char_tamanio);