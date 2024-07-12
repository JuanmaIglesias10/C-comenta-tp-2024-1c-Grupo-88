#include "utils.h"
#include "math.h"
#include "cpu.h"
#include "diccionario_cpu.h"

uint32_t obtener_numero_pagina(uint32_t direccionLogica);

uint32_t obtener_desplazamiento_pagina(uint32_t direccionLogica);

uint32_t calcular_direccion_fisica(uint32_t direccion_logica);

uint32_t pedir_marco_memoria(uint32_t nro_pagina);

uint32_t consultar_TLB(uint32_t nro_pagina);

void actualizar_TLB(uint32_t nro_pagina,uint32_t nro_marco);

bool es_lru();

bool es_fifo();