#include "atender_IO_dialFS.h"

void atender_memoria_IO_DIALFS() {
    while (1) {
        // por ahora no se usa
        sleep(10);
    }
}

void atender_kernel_IO_DIALFS() {
    inicializar_archivos_FS();
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

void inicializar_archivos_FS(){

    // Archivo de Bloques
    /*
    FILE* archBloques = fopen("bloques.dat", "wb");
    if (bloques == NULL) {
        log_error(logger_IO, "Error al crear bloques.dat");
        exit(EXIT_FAILURE);
    }
    ftruncate(fileno(bloques), config_IO_DIALFS.block_size * config_IO_DIALFS.block_count);
    fclose(bloques);
    */
    // Archivo Bitmap
    /*
    FILE* archBitmap = fopen("bitmap.dat", "wb");
    if (bitmap == NULL) {
        log_error(logger_IO, "Error al crear bitmap.dat")
        exit(EXIT_FAILURE);
    }
    */
    int cantBytesNecesarios = config_IO_DIALFS.block_count / 8; // ej: si son 8 bloques necesito 8 bits = 1 byte
    // Inicializar el bitmap con todos los bloques libres
    t_bitarray* bitarray = bitarray_create_with_mode(malloc(cantBytesNecesarios), cantBytesNecesarios, LSB_FIRST);
    //inicializar_bitmap(bitarray);

    // testing
    bitarray_set_bit(bitarray, 0);
    bool valor0 = bitarray_test_bit(bitarray, 0);
    bool valor1 = bitarray_test_bit(bitarray, 1);
    const char* bit0 = bool_to_string(valor0);
    const char* bit1 = bool_to_string(valor1);
    log_info(logger_IO, bit0); // true: esta ocupado
    log_info(logger_IO, bit1); // false: esta libre
    // fin testing

    // bitarray_clean(bitarray);
    //write(bitarray->bitarray, bitarray->size, 1, archBitmap);
    //fclose(archBitmap);

    bitarray_destroy(bitarray);
}

void inicializar_bitmap(t_bitarray* bitarray) {
    for(int i = 0 ; i < config_IO_DIALFS.block_count; i++) {
        bitarray_set_bit(bitarray, i);
    }
}

char* bool_to_string(bool value) {
    return value ? "true" : "false";
}

