#include "atender_IO_dialFS.h"

void atender_memoria_IO_DIALFS() {
    while (1) {
        // por ahora no se usa
        sleep(10);
    }
}

void atender_kernel_IO_DIALFS() {
    testeo_FS();


    while (1) {
        mensajeKernelIO cod_op = recibir_codOp(fd_kernel);
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

    fs_crear_archivo("notas.txt"); // creo un archivo
    //fs_truncate(); // modifo el tamaño del archivo
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

int file_exists_in_directory(const char *directory, const char *nombre_archivo) {
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

int archivos_base_existen() {
    if(file_exists_in_directory(config_IO_DIALFS.path_base_dialfs, "bloques.dat") && 
            file_exists_in_directory(config_IO_DIALFS.path_base_dialfs, "bitmap.dat"))
        return 1;
    else
        return 0;
}

void sincronizar_bitarray() {
    FILE* archivo_bitmap = fopen("/home/utnso/dialfs/bitmap.dat", "ab"); // sobreescribo
    if (archivo_bitmap == NULL) {
        log_error(logger_IO, "Error al abrir bitmap.dat");
        exit(EXIT_FAILURE);
    }
    rewind(archivo_bitmap);
    // LLenar el bitmap con el bitarray que por defecto tiene los bloques libres
    fwrite(bitarray->bitarray, tamanio_bitmap, 1, archivo_bitmap);
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

    
    t_config* config = config_create(path_archivo_metadata);

    int nro_bloque_libre = asignarBloqueLibre();
    // buscar_espacio_contiguo
    // actualizar bitarray

    config_set_value(config, "BLOQUE_INICIAL", string_itoa(nro_bloque_libre));
    config_set_value(config, "TAMANIO_ARCHIVO", "0");

    config_save(config);
    fclose(archivo_metadata);
    free(path_archivo_metadata);
}

int asignarPrimerBloqueLibre() {
    // t_bitarray* bitarray (global)
    for(int i = 0; i < config_IO_DIALFS.block_count; i++) {
        // busco el 1ro que este libre
        if(!bitarray_test_bit(bitarray, i)) {
            bitarray_set_bit(bitarray,i);
            sincronizar_bitarray();
            return i;
        }
    }
    // no hace falta contemplar que no haya ningun bloque libre (creo, atte: nico)
}

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

// para truncate puede que tenga que cambiar el bloque inicial, para eso 1ro debo buscar 
// el primer espacio contiguo de bloques donde quepa el archivo, incluyendo los bloques actuales
// ya asignados

// caso feliz: puedo agrandar el archivo sin mover lo actual
// caso menos feliz: tengo espacio contiguo pero debo cambiar de primer bloque
// caso feo: no tengo espacio contiguo, tengo que compactar

// ya desde el caso menos feliz al feo se podria de entrada liberar los bloques del archivo ya que sabemos que no 
// se puede mantener el bloque inicial y habra que mover de lugar el archivo en el "FS"

// esto es suponiendo que quiera agrandarlo, en caso de achicar seria mas sencillo, si o si voy a tener espacio contiguo