#include "utils.h"
#include "math.h"
#include "diccionario_cpu.h"

int obtener_numero_pagina(int direccionLogica);

int obtener_desplazamiento_pagina(int direccionLogica);

uint32_t calcular_direccion_fisica(int direccion_logica);

uint32_t pedir_marco_memoria(uint32_t nro_pagina);

uint32_t consultar_TLB(uint32_t nro_pagina);

void actualizar_TLB(uint32_t nro_pagina,uint32_t nro_marco);

bool es_lru();

bool es_fifo();