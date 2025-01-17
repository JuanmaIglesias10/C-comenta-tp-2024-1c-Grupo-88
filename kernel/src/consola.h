
#include "diccionario_kernel.h"
#include "kernel.h"
#include <termios.h> // para resetear la consola

void iniciar_consola();
void mostrar_opciones_consola();
void atender_consola(char* input);
t_codigo_operacion get_codigo_operacion(char* comando, int cant_par);
void switch_comandos(uint8_t codOp, char* lista_mensaje);

void liberar_lista_script(t_list* lista);


void reset_terminal_mode();