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

    tamanio_bitmap = config_IO_DIALFS.block_count / 8 ; // ej: si son 8 bloques necesito 8 bits = 1 byte
    crear_bitarray();
    lista_metadata_archivos = list_create();
    if(!archivos_base_existen()) { // si no existen los archivos base
        crear_archivos_base();

        fs_crear_archivo("manzanas.txt");
        fs_truncar_archivo("manzanas.txt",10);
    }
    else { // si ya exixten los archivos base
        leer_bitmap();
        leer_metadatas();
        //list_remove();
        t_config* config_aux = list_get(lista_metadata_archivos,0);
        int value = config_get_int_value(config_aux,"TAMANIO_ARCHIVO");
        printf("%d", value);
    }

    
    // fs_crear_archivo("peras.txt");
    // fs_truncar_archivo("peras.txt",12);

    // fs_truncar_archivo("manzanas.txt",18);

    // leer_bitmap();
    // mostrar_bitarray(bitarray);



    
    // fs_write(); // leo algo de la RAM -> lo escribo en el archivo
    // fs_read(); // leo de un archivo -> lo escribo en la RAM
    // fs_delete(); // elimino un archivo
}

void crear_bitarray() {
    // Crear e inicializar el bitarray con todos los bloques libres
    bitarray = bitarray_create_with_mode(malloc(tamanio_bitmap), tamanio_bitmap, LSB_FIRST);
    bitarray_clean(bitarray);
}

void crear_archivos_base(){
    // Crear archivo de Bloques
    FILE* archBloques = fopen("/home/utnso/dialfs/bloques.dat", "wb"); // lo creo
    // Establecer tamaño del archivo de Bloques
    ftruncate(fileno(archBloques), config_IO_DIALFS.block_size * config_IO_DIALFS.block_count);
    fclose(archBloques);
    
    // Crear archivo Bitmap
    FILE* archBitmap = fopen("/home/utnso/dialfs/bitmap.dat", "wb"); // lo creo
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
    //fflush(stdout);
    printf("Mapa de bloques libres\n");
    for(int i = 0; i < config_IO_DIALFS.block_count; i++) {
        bool bit = bitarray_test_bit(unBitarray, i);
        char* valorBit = bool_to_string(bit);
        //fflush(stdout);
        printf("Bloque %d: %s\n",i,valorBit);
    }

    // muestra un pedazo de memoria por pantalla en forma hexa
    // mem_hexdump(bitarray->bitarray,bitarray->size);
}

void mostrar_bitmap() {
    // TESTING: Mostrar el bitmap
    FILE* archBitmapTest = fopen("/home/utnso/dialfs/bitmap.dat", "r+b"); // lo abro como read
    t_bitarray* bitarrayLeido = bitarray_create_with_mode(malloc(tamanio_bitmap), tamanio_bitmap, LSB_FIRST);
    rewind(archBitmapTest);
    fread(bitarrayLeido->bitarray, tamanio_bitmap, 1, archBitmapTest);   
    mem_hexdump(bitarrayLeido->bitarray,bitarrayLeido->size); 
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

void leer_metadatas(){
    char* directory = config_IO_DIALFS.path_base_dialfs;
    struct dirent *entry;
    DIR *dp = opendir(directory);

    while ((entry = readdir(dp))) {
        if ((strcmp(entry->d_name, "bloques.dat") != 0) && (strcmp(entry->d_name, "bitmap.dat") != 0)) {
            char* path_archivo_metadata = string_new();
            string_append(&path_archivo_metadata, config_IO_DIALFS.path_base_dialfs);
            string_append(&path_archivo_metadata, entry->d_name); 

            t_config* config_md = config_create(path_archivo_metadata);
            list_add(lista_metadata_archivos, config_md);

            free(path_archivo_metadata);
        }
    }
    closedir(dp);
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

void actualizar_bitmap() {
    FILE* archivo_bitmap = fopen("/home/utnso/dialfs/bitmap.dat", "r+b");
    fwrite(bitarray->bitarray, tamanio_bitmap, 1, archivo_bitmap);
    fclose(archivo_bitmap);
    log_debug(logger_IO, "bitmap actualizado");
}

void leer_bitmap() {
    FILE* archivo_bitmap = fopen("/home/utnso/dialfs/bitmap.dat", "rb");
    fread(bitarray->bitarray, tamanio_bitmap, 1, archivo_bitmap);
    fclose(archivo_bitmap);
    log_debug(logger_IO, "bitmap leido");
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
    actualizar_bitmap();

    config_set_value(config_md, "BLOQUE_INICIAL", string_itoa(nro_bloque_libre));
    config_set_value(config_md, "TAMANIO_ARCHIVO", "0");

    list_add(lista_metadata_archivos, config_md);
    config_save(config_md);
    //config_destroy(config_md);
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
    else {
        //double aux = double(tamanio_actual / tamanio_bloque);
        cantidad_bloques_actual = ceil((double)tamanio_actual / (double)tamanio_bloque);
    }
    uint32_t cantidad_bloques_nueva;
    if (nuevo_tamanio == 0) 
        cantidad_bloques_nueva = 1;
    else 
        cantidad_bloques_nueva = ceil((double)nuevo_tamanio / (double)tamanio_bloque);
        
    uint32_t ultimo_bloque_actual = bloque_inicial + cantidad_bloques_actual - 1;
    uint32_t ultimo_bloque_nuevo = bloque_inicial + cantidad_bloques_nueva - 1;

    // caso 3
    uint32_t espacio_libre_ult_bloque = cantidad_bloques_actual * tamanio_bloque - tamanio_actual;

    // caso 4 y 5
    uint32_t cantidad_bloques_a_agregar = cantidad_bloques_nueva - cantidad_bloques_actual; // caso 4 y 5
    uint32_t cant_bloques_siguientes_libres = contar_bloques_siguientes_libres(ultimo_bloque_actual); // caso 4

    uint32_t nuevo_bloque_inicial = hay_espacio_contiguo_libre_en_algun_lado(cantidad_bloques_nueva); // caso 5

    // caso 1: si el tamanio es igual al actual
    if (nuevo_tamanio == tamanio_actual) {
        // no hago nada
        log_debug(logger_IO,"Truncar. Caso 1: El tamanio es el mismo.");
    }

    // REDUCIR ARCHIVO

    // caso 2 : si el tamanio es mas chico al actual
    else if (nuevo_tamanio<tamanio_actual) {
        log_debug(logger_IO,"Truncar. Caso 2: Reducir el tamanio.");
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
        actualizar_bitmap();
    }

    // FIN REDUCIR ARCHIVO 

    // AMPLIAR ARCHIVO

    /* Caso 3: tengo espacio en el ultimo bloque*/
    else if (nuevo_tamanio < tamanio_actual + espacio_libre_ult_bloque) {
        log_debug(logger_IO,"Truncar. Caso 3. Ampliar en el ultimo bloque");
        // cambiar tamanio
        config_set_value(config_md, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
        config_save(config_md);
    }

    /* Caso 4: tengo espacio contiguo suficiente despues del ultimo bloque actual para ampliar */
    else if (cantidad_bloques_a_agregar <= cant_bloques_siguientes_libres) {
        log_debug(logger_IO,"Truncar. Caso 4: Ampliar al final");

        // bloque inicial no cambia
        // cambiar tamanio
        config_set_value(config_md, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
        config_save(config_md);

        // asigno los bloques 
        for(int i = ultimo_bloque_actual + 1; i <= ultimo_bloque_nuevo; i++) {
            bitarray_set_bit(bitarray,i); // setea en 1 = ocupado
        }
        actualizar_bitmap();        
    }

    // Caso 5: tengo espacio contiguo en el FS pero no al final del archivo. No hay que compactar porque entra pero en otro lado
    else if (nuevo_bloque_inicial != -1) {
        log_debug(logger_IO,"Truncar. Caso 5: Mover para ampliar");

        ultimo_bloque_nuevo = nuevo_bloque_inicial + cantidad_bloques_nueva - 1;
        
        // cambiar tamanio y bloque inicial
        config_set_value(config_md, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
        config_set_value(config_md, "BLOQUE_INICIAL", string_itoa(nuevo_bloque_inicial));
        config_save(config_md);

        // mover "archivo" de lugar dentro del archivo de bloques
        FILE* arch_bloques = fopen("/home/utnso/dialfs/bloques.dat", "r+b"); // El puntero se posiciona al inicio
        char* buffer = (char*)malloc(tamanio_actual);
        fseek(arch_bloques, bloque_inicial * tamanio_bloque, SEEK_SET);
        fread(buffer, tamanio_actual, 1, arch_bloques);
        fseek(arch_bloques, nuevo_bloque_inicial * tamanio_bloque, SEEK_SET);
        fwrite(buffer, tamanio_actual, 1, arch_bloques);
        free(buffer);
        fclose(arch_bloques);

        // liberar bloques
        for(int i = bloque_inicial; i <= ultimo_bloque_actual; i++) {
            bitarray_clean_bit(bitarray,i); // lo marco libre
        }
        // asignar bloques
        for(int i = nuevo_bloque_inicial; i <= ultimo_bloque_nuevo; i++) {
            bitarray_set_bit(bitarray,i); // lo marco lleno
        }
        actualizar_bitmap();
    }

    // caso 6: no tengo espacio contiguo en el FS, debo compactar
    else {
        /*
        Luego de compactar podria volver a llamar a esta funcion ya que habria que volver a ver que 
        */

        // leo el contenido del archivo
        FILE* arch_bloques = fopen("/home/utnso/dialfs/bloques.dat", "r+b"); // El puntero se posiciona al inicio
        char* buffer = (char*)malloc(tamanio_actual);
        fseek(arch_bloques, bloque_inicial * tamanio_bloque, SEEK_SET);
        fread(buffer, tamanio_actual, 1, arch_bloques);
        fclose(arch_bloques);

        // libero bloques actuales
        for(int i = bloque_inicial; i <= ultimo_bloque_actual; i++) {
            bitarray_clean_bit(bitarray,i); // lo marco libre
        }

        // compacto
        //compactar_FS();

        // busco nueva posicion para el archivo inicial
        nuevo_bloque_inicial = hay_espacio_contiguo_libre_en_algun_lado(cantidad_bloques_nueva);
    
        // escribo el contenido del archivo en el nuevo lugar
        arch_bloques = fopen("/home/utnso/dialfs/bloques.dat", "r+b"); // El puntero se posiciona al inicio
        fseek(arch_bloques, nuevo_bloque_inicial * tamanio_bloque, SEEK_SET);
        fwrite(buffer, tamanio_actual, 1, arch_bloques);
        fclose(arch_bloques);
        free(buffer);

        ultimo_bloque_nuevo = nuevo_bloque_inicial + cantidad_bloques_nueva - 1;

        // asigno bloques nuevos
        for(int i = nuevo_bloque_inicial; i <= ultimo_bloque_nuevo; i++) {
            bitarray_set_bit(bitarray,i); // lo marco lleno
        }
        actualizar_bitmap();
    }

    // FIN AMPLIAR ARCHIVO

    //log_info(logger_IO, "PID: <PID> - Truncar Archivo: %s - Tamaño: %u", nombre_archivo, nuevo_tamanio); // LOG OBLIGATORIO
}

// void compactar_FS() {
    

//     mover_a_primer_lugar_libre(archivo);

// }


uint32_t contar_bloques_siguientes_libres(uint32_t ultimo_bloque_actual) {
    // bibitarray: global
    
    uint32_t cantidad_bloques_libres = 0;
    uint32_t i = ultimo_bloque_actual + 1;
    
    while ( i <= (config_IO_DIALFS.block_count -1) && ! bitarray_test_bit(bitarray,i)) {
        cantidad_bloques_libres ++;
        i ++;
    }
    
    return cantidad_bloques_libres;
}

uint32_t hay_espacio_contiguo_libre_en_algun_lado (uint32_t cantidad_bloques_nueva) {
    
    uint32_t cantidad_bloques_libres_contiguos = 0;
    uint32_t i = 0;
    uint32_t pos_primer_bloque_contiguo_libre;

    while ( i <= (config_IO_DIALFS.block_count - 1 ) && cantidad_bloques_libres_contiguos != cantidad_bloques_nueva) {

        // si esta ocupado
        if (bitarray_test_bit(bitarray,i)) {
            i++;
            cantidad_bloques_libres_contiguos = 0;
        }

        //si esta libre
        else {
            if (cantidad_bloques_libres_contiguos == 0) 
                pos_primer_bloque_contiguo_libre = i;
            cantidad_bloques_libres_contiguos ++;
            i++;
        }
    }

    if (cantidad_bloques_libres_contiguos == cantidad_bloques_nueva) 
        return pos_primer_bloque_contiguo_libre;
    else 
        return -1; //no hay bloques contiguos libre suficientes para mover al archivo -> hay que hacer compactacion
}

/*
typedef struct{
    char* path_base_dialfs;
    int block_size;
    int block_count;
    int retraso_compactacion;
} t_config_IO_DIALFS;
*/