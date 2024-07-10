#include "atender_IO_dialFS.h"

void atender_memoria_IO_DIALFS() {
    while (1) {
        // por ahora no se usa
        sleep(10);
    }
}

void atender_kernel_IO_DIALFS() {
    testeo_FS(); // luego cambiar

    while (1) {
        mensajeKernelIO cod_op = recibir_codOp(fd_kernel);

        // recibir PID
        // recibir nombre del archivo

        switch (cod_op) {
            case FS_CREATE:
                // TODO
                break;
            case FS_DELETE:
                // TODO
                break;
            case FS_TRUNCATE:
                // TODO
                break;
            case FS_WRITE:
                // TODO
                break;
            case FS_READ:
                // TODO
                break;
            default:
                return;
        }
    }
}

/*

1. Inicializar los archivos de bloques y el bitmap.
2. Implementar funciones CREATE, DELETE, TRUNCATE, RWRITE, READ
3. Implementar la compactación del sistema de archivos.

Libreria Bitarray:
https://github.com/sisoputnfrba/so-commons-library/blob/master/src/commons/bitarray.h

*/

void testeo_FS() {
    crear_bitarray();
    if(!archivos_base_existen()) {
        crear_archivos_base();
    }
    sincronizar_bitarray();
    mostrar_bitmap(); // TESTING

    fs_crear_archivo("notas.txt");
    fs_truncar_archivo("notas.txt",15);


    //fs_write(); // leo algo de la RAM -> lo escribo en el archivo
    //fs_read(); // leo de un archivo -> lo escribo en la RAM
    //fs_delete(); // elimino un archivo
}

void crear_bitarray() {
    // ambas son variables globables

    // calcular cantidad de bytes necesarios para el bitarray y el bitmap (deberian pesar lo mismo)
    // tener en cuenta que hablamos del bitarray como tal, que esta dentro de t_bitarray
    tamanio_bitmap = config_IO_DIALFS.block_count / 8 ; // ej: si son 8 bloques necesito 8 bits = 1 byte

    // Crear e inicializar el bitarray con todos los bloques libres
    bitarray = bitarray_create_with_mode(malloc(tamanio_bitmap), tamanio_bitmap, LSB_FIRST);
    bitarray_clean(bitarray);
}

void crear_archivos_base(){

    // Crear archivo de Bloques
    FILE* archBloques = fopen("/home/utnso/dialfs/bloques.dat", "wb"); // lo creo
    if (archBloques == NULL) {
        log_error(logger_IO, "Error al crear bloques.dat");
        exit(EXIT_FAILURE);
    }

    // Establecer tamaño del archivo de Bloques
    ftruncate(fileno(archBloques), config_IO_DIALFS.block_size * config_IO_DIALFS.block_count);
    fclose(archBloques);
    
    // Crear archivo Bitmap
    FILE* archBitmap = fopen("/home/utnso/dialfs/bitmap.dat", "wb"); // lo creo
    if (archBitmap == NULL) {
        log_error(logger_IO, "Error al crear bitmap.dat");
        exit(EXIT_FAILURE);
    }
    
    // Establecer tamaño del archivo bitmap
    ftruncate(fileno(archBitmap), tamanio_bitmap);
    fclose(archBitmap);

    log_debug(logger_IO, "archivos base creados");
}

void bitarray_clean(t_bitarray* bitarray) {
    for(int i = 0 ; i < config_IO_DIALFS.block_count; i++) {
        bitarray_clean_bit(bitarray, i); // pongo en 0 (libre) el bit
    }
}

void mostrar_bitarray(t_bitarray* unBitarray) {
    // TESTING:
    fflush(stdout);
    printf("Mapa de bloques libres\n");
    for(int i = 0; i < config_IO_DIALFS.block_count; i++) {
        bool bit = bitarray_test_bit(unBitarray, i);
        char* valorBit = bool_to_string(bit);
        fflush(stdout);
        printf("Bloque %d: %s\n",i,valorBit);
    }

    // muestra un pedazo de memoria por pantalla en forma hexa
    // mem_hexdump(bitarray->bitarray,bitarray->size);
}

void mostrar_bitmap() {
    // TESTING: Mostrar el bitmap
    FILE* archBitmapTest = fopen("/home/utnso/dialfs/bitmap.dat", "rb"); // lo abro como read
    t_bitarray* bitarrayLeido = bitarray_create_with_mode(malloc(tamanio_bitmap), tamanio_bitmap, LSB_FIRST);
    fread(bitarrayLeido->bitarray, tamanio_bitmap, 1, archBitmapTest);    
    mostrar_bitarray(bitarrayLeido);
}

char* bool_to_string(bool value) {
    return value ? "ocupado" : "libre";
}

bool existe_archivo(char* nombre_archivo) {
    char* directory = config_IO_DIALFS.path_base_dialfs;
    
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("opendir");
        return 0; // No se pudo abrir el directorio
    }

    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, nombre_archivo) == 0) {
            closedir(dp);
            return 1; // El archivo existe en el directorio
        }
    }

    closedir(dp);
    return 0; // El archivo no existe en el directorio
}

bool archivos_base_existen() {
    if(existe_archivo("bloques.dat") && existe_archivo("bitmap.dat")) {
        return 1;
        log_debug(logger_IO, "archivos base estan presentes");
    }
    else {
        return 0;
    }
}

void sincronizar_bitarray() {
    FILE* archivo_bitmap = fopen("/home/utnso/dialfs/bitmap.dat", "ab");
    if (archivo_bitmap == NULL) {
        log_error(logger_IO, "Error al abrir bitmap.dat");
        exit(EXIT_FAILURE);
    }
    rewind(archivo_bitmap);
    // Sobrescribo el bitmap con el bitarray
    fwrite(bitarray->bitarray, tamanio_bitmap, 1, archivo_bitmap);

    log_debug(logger_IO, "bitmap sincronizado");
}

void fs_crear_archivo(char* nombre_archivo) {
    // Crear Archivo Metadata

    char* path_archivo_metadata = string_new();
    string_append(&path_archivo_metadata, config_IO_DIALFS.path_base_dialfs);
    string_append(&path_archivo_metadata, nombre_archivo); 

    
    FILE* archivo_metadata = fopen(path_archivo_metadata, "wb"); // lo creo
    if (archivo_metadata == NULL) {
        log_error(logger_IO, "Error al crear archivo de metadata de %s",nombre_archivo);
        exit(EXIT_FAILURE);
    }

    t_config* config_md = config_create(path_archivo_metadata);

    int nro_bloque_libre = encontrarPrimerBloqueLibre();
    bitarray_set_bit(bitarray,nro_bloque_libre);
    sincronizar_bitarray();

    config_set_value(config_md, "BLOQUE_INICIAL", string_itoa(nro_bloque_libre));
    config_set_value(config_md, "TAMANIO_ARCHIVO", "0");

    config_save(config_md);
    config_destroy(config_md);
    fclose(archivo_metadata);
    free(path_archivo_metadata);

    log_info(logger_IO, "PID: <PID> - Crear Archivo: %s", nombre_archivo); // LOG OBLIGATORIO // to do: agregar pid

    // los archivos no hace falta vincularlos a un PID luego de creados
    // un proceso puede interactuar con los archivos de otros
}

int encontrarPrimerBloqueLibre() {
    // t_bitarray* bitarray (global)
    for(int i = 0; i < config_IO_DIALFS.block_count; i++) {
        // busco el 1ro que este libre
        if(!bitarray_test_bit(bitarray, i)) {
            return i;
        }
    }
    // no hace falta contemplar que no haya ningun bloque libre (creo, atte: nico)
    log_error(logger_IO,"no se encontraron bloques libres");
    return -1;
}

t_config* obtener_metadata(char* nombre_archivo) {
    char* path_archivo_metadata = string_new();
    string_append(&path_archivo_metadata, config_IO_DIALFS.path_base_dialfs);
    string_append(&path_archivo_metadata, nombre_archivo); 
    t_config* config_md = config_create(path_archivo_metadata);
    return config_md;
}

uint32_t tamanio_archivo(char* nombre_archivo) {
    t_config* config_md = obtener_metadata(nombre_archivo);
    uint32_t bloque_inicial_archivo = config_get_int_value(config_md, "TAMANIO_ARCHIVO");
    config_destroy(config_md);
    return bloque_inicial_archivo;
}

uint32_t bloque_inicial_archivo(char* nombre_archivo) {
    t_config* config_md = obtener_metadata(nombre_archivo);
    uint32_t tamanio = config_get_int_value(config_md, "BLOQUE_INICIAL");
    config_destroy(config_md);
    return tamanio;
}

void fs_truncar_archivo(char* nombre_archivo, uint32_t nuevo_tamanio) {

    int tamanio_bloque = config_IO_DIALFS.block_size;
    //int cantidad_bloques_FS = config_IO_DIALFS.block_count;
    t_config* config_md = obtener_metadata(nombre_archivo);
    uint32_t bloque_inicial = config_get_int_value(config_md, "BLOQUE_INICIAL");
    uint32_t tamanio_actual = config_get_int_value(config_md, "TAMANIO_ARCHIVO");

    uint32_t cantidad_bloques_actual;
    if (tamanio_actual == 0) 
        cantidad_bloques_actual = 1;
    else 
        cantidad_bloques_actual = ceil(tamanio_actual / tamanio_bloque);

    uint32_t cantidad_bloques_nueva;
    if (nuevo_tamanio == 0) 
        cantidad_bloques_nueva = 1;
    else 
        cantidad_bloques_nueva = ceil(nuevo_tamanio / tamanio_bloque);
        
    uint32_t ultimo_bloque_actual = bloque_inicial + cantidad_bloques_actual - 1;
    uint32_t ultimo_bloque_nuevo = bloque_inicial + cantidad_bloques_nueva - 1;

    uint32_t espacio_libre_ult_bloque = cantidad_bloques_actual * tamanio_bloque - tamanio_actual;

    // caso 1: si el tamanio es igual al actual
    if (nuevo_tamanio == tamanio_actual) {
        // no hago nada
        log_debug(logger_IO,"Truncar: Caso 1");
    }

    // REDUCIR ARCHIVO

    // caso 2 : si el tamanio es mas chico al actual
    else if (nuevo_tamanio<tamanio_actual) {
        log_debug(logger_IO,"Truncar: Caso 2");
        // bibitarray: global

        // el bloque inicial no va a necesitar cambiar

        // cambiar tamanio
        config_set_value(config_md, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
        config_save(config_md);

        // liberar los bloques ya no necesarios
        // ejemplo: 0 1 2 3 4 5 -- ''6 7 8'' 9 10 -- 11 12 13 14 15
        for(int i = ultimo_bloque_actual; i > ultimo_bloque_nuevo; i--) {
            bitarray_clean_bit(bitarray,i); 
        }
        sincronizar_bitarray();
    }

    // FIN REDUCIR ARCHIVO 

    // AMPLIAR ARCHIVO

    /* Caso 3: tengo espacio en el ultimo bloque*/
    else if (nuevo_tamanio < tamanio_actual + espacio_libre_ult_bloque) {
        log_debug(logger_IO,"Truncar: Caso 3");
        // cambiar tamanio
        config_set_value(config_md, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
        config_save(config_md);
    }

    

    // else if (/*si no tengo espacio libre*/) {

    //     // hace falta? creo que no

    // }

    // // a partir de aca se supone hay espacio libre en el FS para ampliarlo

    // else if (/*tengo espacio contiguo al final del archivo para ampliar*/) {

    //     // -> (bloque inicial no cambia) -> parecido al anterior pero al reves

    //     // calculo cuantos bloques libres hay despues del archivo

    //     estado_bloque = bitarray_test_bit;

    //     while()


    // }

    // else if (/*no tengo espacio contiguo al final pero si en el FS*/) {
    //     // (cambia el bloque inicial y debo mover bloques actuales)
    // }

    // else (/*no tengo espacio contiguo en el FS*/) {
    //     /*
    //     -> tengo que compactar
    //     /duda: deberia compactar incluido el archivo si ya se que lo voy a tener que mover al final? me quedaria el hueco libre
    //     lo ideal seria primero quitar el archivo que voy a ampliar y luego compactar en si, 
    //     y agregar al final el archivo ampliado despues de compactar (se podria )

    //     Luego de compactar podria volver a llamar a esta funcion ya que habria que volver 

    //     */
    // }

    // FIN AMPLIAR ARCHIVO

    // log_info(logger_IO, "PID: <PID> - Truncar Archivo: %s - Tamaño: %u", nombre_archivo, nuevo_tamanio); // LOG OBLIGATORIO
}



/*
typedef struct{
    char* tipo_interfaz;
    int tiempo_unidad_trabajo;
    char* ip_kernel;
    int puerto_kernel;
    char* ip_memoria;
    int puerto_memoria;
    char* path_base_dialfs;
    int block_size;
    int block_count;
    int retraso_compactacion;
} t_config_IO_DIALFS;
*/


// para truncate puede que tenga que cambiar el bloque inicial, para eso 1ro debo buscar 
// el primer espacio contiguo de bloques donde quepa el archivo, incluyendo los bloques actuales ya asignados

// caso feliz: puedo agrandar el archivo sin mover lo actual
// caso menos feliz: tengo espacio contiguo pero debo cambiar de primer bloque
// caso feo: no tengo espacio contiguo, tengo que compactar

// ya desde el caso menos feliz al feo se podria de entrada liberar los bloques del archivo ya que sabemos que no 
// se puede mantener el bloque inicial y habra que mover de lugar el archivo en el "FS"

// esto es suponiendo que quiera agrandarlo, en caso de achicar seria mas sencillo, si o si voy a tener espacio contiguo



/*
int buscar_espacio_contiguo(tamanio){
    // para cuando hago truncate

    // tengo que el tamanio contiguo que necesito dividirlo por el tamanio
    // de bloque para saber la cantidad de bloques necesarios
    

    // devuelvo el nro de bloque
}*/

/*
int buscar_espacio_contiguo_desde(nro_bloque){
    // para cuando hago truncate

    // tengo que el tamanio contiguo que necesito dividirlo por el tamanio
    // de bloque para saber la cantidad de bloques necesarios
    

    // devuelvo el nro de bloque
}*/