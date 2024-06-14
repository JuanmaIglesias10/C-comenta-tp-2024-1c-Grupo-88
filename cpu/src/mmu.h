#include "utils.h"
#include "math.h"
#include "diccionario_cpu.h"

int obtener_numero_pagina(int direccionLogica);

int obtener_desplazamiento_pagina(int direccionLogica);

uint32_t calcular_direccion_fisica(int direccion_logica, t_cde* cde);