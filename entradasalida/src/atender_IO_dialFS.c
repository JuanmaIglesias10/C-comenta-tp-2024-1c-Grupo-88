#include "atender_IO_dialFS.h"

void atender_kernel_IO_DIALFS() {
    // inicializar_FS();
    testeo_FS();

    // while (1) {
        // mostrar_info_archivos();
        // char* valor_leido = fs_leer_archivo("salida.txt", 0, 69);

    //     mensajeKernelIO cod_op = recibir_codOp(fd_kernel);

    //     switch (cod_op) {
    //         case FS_CREATE:
    //             mostrar_info_archivos();
    //             ejecutar_IO_FS_CREATE();
    //             mostrar_info_archivos();
    //             break;
    //         case FS_DELETE:
    //             mostrar_info_archivos();
    //             ejecutar_IO_FS_DELETE();
    //             mostrar_info_archivos();
    //             break;
    //         case FS_TRUNCATE:
    //             mostrar_info_archivos();
    //             ejecutar_IO_FS_TRUNCATE();
    //             mostrar_info_archivos();
    //             break;
    //         case FS_WRITE:
    //             mostrar_info_archivos();
    //             ejecutar_IO_FS_WRITE();
    //             mostrar_info_archivos();
    //             break;
    //         case FS_READ:
    //             mostrar_info_archivos();
    //             ejecutar_IO_FS_READ();
    //             mostrar_info_archivos();
    //             break;
    //         default:
    //             log_warning(logger_IO, "CODIGO DE OPERACION NO RECONOCIDO");
    //             return;
    //      }
    // }
}

void testeo_FS() {

    tamanio_bitmap = config_IO_DIALFS.block_count / 8 ; // ej: si son 8 bloques necesito 8 bits = 1 byte
    crear_bitarray();
    lista_info_archivos = list_create();
    if(!archivos_base_existen()) { // si no existen los archivos base
        crear_archivos_base(); // obligatorio

        mostrar_info_archivos();

        // FS_1
        fs_crear_archivo("salida.txt"); // bloque 0 (desactualizado)
        fs_crear_archivo("cronologico.txt"); // bloque 1 (desactualizado)
        fs_truncar_archivo("salida.txt", 80); // bloque 2 al 6
        fs_truncar_archivo("cronologico.txt", 80); // bloque 7 al 11
        fs_escribir_archivo("salida.txt", 0, 69, "Fallout 1 Fallout 2 Fallout 3 Fallout: New Vegas Fallout 4 Fallout 76");
        fs_escribir_archivo("cronologico.txt", 0, 69, "Fallout 76 Fallout 1 Fallout 2 Fallout 3 Fallout: New Vegas Fallout 4");

        // FS_2
        fs_crear_archivo("archivo1.txt"); // bloque 0 (desactualizado)
        fs_crear_archivo("archivo2.txt"); // bloque 1 (desactualizado)
        fs_truncar_archivo("archivo1.txt", 10); // bloque 0
        fs_truncar_archivo("archivo2.txt", 10); // bloque 1

        mostrar_info_archivos();
        mostrar_bitarray(bitarray);

        // char* cadena_leida;
        // cadena_leida = fs_leer_archivo("salida.txt", 0, 69);
        // printf("Cadena leída: %s\n", cadena_leida);
        // cadena_leida = fs_leer_archivo("cronologico.txt", 0, 69);
        // printf("Cadena leída: %s\n", cadena_leida);
        
    }
    else { // si ya exixten los archivos base
        leer_bitmap(); // obligatorio
        leer_info_archivos(); // obligatorio

        char* cadena_leida;
        cadena_leida = fs_leer_archivo("salida.txt", 0, 69);
        printf("Cadena leída: %s\n", cadena_leida);
        cadena_leida = fs_leer_archivo("cronologico.txt", 0, 69);
        printf("Cadena leída: %s\n", cadena_leida);

    }

    printf("\n\n");
}

void inicializar_FS() {
    tamanio_bitmap = config_IO_DIALFS.block_count / 8 ; // ej: si son 8 bloques necesito 8 bits = 1 byte
    crear_bitarray();
    lista_info_archivos = list_create();
    if(!archivos_base_existen()) { // si no existen los archivos base
        crear_archivos_base(); // obligatorio

        //mostrar_bitarray(bitarray);
        //mostrar_info_archivos();
    }
    else { // si ya exixten los archivos base
        leer_bitmap(); // obligatorio
        leer_info_archivos(); // obligatorio

        //mostrar_bitarray(bitarray);
        //mostrar_info_archivos();
    }

    printf("\n\n");
}

void ejecutar_IO_FS_CREATE() {
    t_buffer* buffer_recibido =  recibir_buffer(fd_kernel);
    uint32_t pid = leer_buffer_uint32(buffer_recibido);
    char* nombre_archivo = leer_buffer_string(buffer_recibido);

    fs_crear_archivo(nombre_archivo);

    log_info(logger_IO, "PID: %d - Crear Archivo: %s", pid, nombre_archivo); // LOG OBLIGATORIO
    enviar_codOp(fd_kernel, FS_CREATE_OK);
}

void ejecutar_IO_FS_DELETE() {
    t_buffer* buffer_recibido =  recibir_buffer(fd_kernel);
    uint32_t pid = leer_buffer_uint32(buffer_recibido);
    char* nombre_archivo = leer_buffer_string(buffer_recibido);

    fs_eliminar_archivo(nombre_archivo);

    log_info(logger_IO, "PID: %d - Eliminar Archivo: %s", pid, nombre_archivo); // LOG OBLIGATORIO
    enviar_codOp(fd_kernel, FS_DELETE_OK);
}
    
void ejecutar_IO_FS_TRUNCATE(){
    t_buffer* buffer_recibido =  recibir_buffer(fd_kernel);
    uint32_t pid = leer_buffer_uint32(buffer_recibido);
    char* nombre_archivo = leer_buffer_string(buffer_recibido);
    uint32_t nuevo_tamanio = leer_buffer_uint32(buffer_recibido);

    fs_truncar_archivo(nombre_archivo, nuevo_tamanio);

    log_info(logger_IO, "PID: %d - Truncar Archivo: %s - Tamaño: %u", pid, nombre_archivo, nuevo_tamanio); // LOG OBLIGATORIO
    enviar_codOp(fd_kernel, FS_TRUNCATE_OK);
}

void ejecutar_IO_FS_WRITE() {
    t_buffer* buffer_recibido =  recibir_buffer(fd_kernel);
    uint32_t pid = leer_buffer_uint32(buffer_recibido);
    char* nombre_archivo = leer_buffer_string(buffer_recibido);
    uint32_t direccion_fisica = leer_buffer_uint32(buffer_recibido); // enviar a memoria
    uint32_t tamanio_a_escribir = leer_buffer_uint32(buffer_recibido);
    uint32_t puntero_archivo = leer_buffer_uint32(buffer_recibido);
	destruir_buffer(buffer_recibido);

    enviar_codOp(fd_memoria, IO_M_FS_WRITE_SOLICITUD);
	t_buffer* buffer = crear_buffer();
    agregar_buffer_uint32(buffer,pid);
    agregar_buffer_uint32(buffer,direccion_fisica);
    agregar_buffer_uint32(buffer,tamanio_a_escribir);
    enviar_buffer(buffer,fd_memoria); 
    destruir_buffer(buffer);

    mensajeIOMemoria cod_op = recibir_codOp(fd_memoria);

    if(cod_op == IO_M_FS_WRITE_OK) {
        t_buffer* buffer_string = recibir_buffer(fd_memoria);
        char* string_recibido = leer_buffer_string(buffer_string);
        destruir_buffer(buffer_string);

        log_warning(logger_IO, "%s" , string_recibido);
        //Pongo el string en el archivo
        fs_escribir_archivo(nombre_archivo, puntero_archivo, tamanio_a_escribir, string_recibido);

        enviar_codOp(fd_kernel, FS_WRITE_OK);
        log_info(logger_IO, "PID: %d - Escribir Archivo: %s - Tamaño a Escribir: %d - Puntero Archivo: %d", pid, nombre_archivo, tamanio_a_escribir, puntero_archivo);
    }
}

void ejecutar_IO_FS_READ() {
    t_buffer* buffer_recibido = recibir_buffer(fd_kernel);
    uint32_t pid = leer_buffer_uint32(buffer_recibido);
    char* nombre_archivo = leer_buffer_string(buffer_recibido);
    uint32_t direccion_fisica = leer_buffer_uint32(buffer_recibido);
    uint32_t tamanio_a_leer = leer_buffer_uint32(buffer_recibido);
    uint32_t puntero_archivo = leer_buffer_uint32(buffer_recibido);
    destruir_buffer(buffer_recibido);
    
    char* valor_leido = fs_leer_archivo(nombre_archivo, puntero_archivo, tamanio_a_leer);

    enviar_codOp(fd_memoria, IO_M_FS_READ_SOLICITUD);
    t_buffer* buffer = crear_buffer();
    agregar_buffer_uint32(buffer, pid);
    agregar_buffer_uint32(buffer, direccion_fisica);
    agregar_buffer_string(buffer, valor_leido);
    agregar_buffer_uint32(buffer, tamanio_a_leer);
    enviar_buffer(buffer, fd_memoria);

    mensajeIOMemoria cod_op = recibir_codOp(fd_memoria);
    
    if(cod_op == IO_M_FS_READ_OK) {
        enviar_codOp(fd_kernel, FS_READ_OK);
        log_info(logger_IO, "PID: %d - Leer Archivo: %s - Tamaño a Leer: %d - Puntero Archivo: %d", pid, nombre_archivo, tamanio_a_leer, puntero_archivo);
    } 
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

void leer_info_archivos(){
    char* directory = config_IO_DIALFS.path_base_dialfs;
    struct dirent *entry;
    DIR *dp = opendir(directory);
    printf("Lectura inicial de archivos en el FS:\n");
    while ((entry = readdir(dp))) {
        char* nombre_archivo = entry->d_name;
        // ignorar las entradas de directorio actual y directorio padre
        if (strcmp(nombre_archivo, ".") == 0 || strcmp(nombre_archivo, "..") == 0) {
            continue;
        } 
        if ((strcmp(nombre_archivo, "bloques.dat") != 0) && (strcmp(nombre_archivo, "bitmap.dat") != 0)) {
            printf("- Archivo encontrado: %s\n", nombre_archivo);
            char* path_archivo_metadata = string_new();
            string_append(&path_archivo_metadata, config_IO_DIALFS.path_base_dialfs);
            string_append(&path_archivo_metadata, nombre_archivo); 

            t_config* config_md = config_create(path_archivo_metadata);
            t_info_archivo* info_archivo = (t_info_archivo*)malloc(sizeof(t_info_archivo));
            info_archivo->nombre_archivo = nombre_archivo;
            info_archivo->config_archivo = config_md;

            list_add(lista_info_archivos, info_archivo);

            free(path_archivo_metadata);
        }
    }
    closedir(dp);
}

void mostrar_info_archivos() {
    printf("Mostrando informacion de los archivos en el FS\n");
    for(int i = 0; i < list_size(lista_info_archivos); i++) {
        t_info_archivo* info_archivo = list_get(lista_info_archivos,i);
        t_config* config_archivo = info_archivo->config_archivo;
        int bloque = config_get_int_value(config_archivo,"BLOQUE_INICIAL");
        int tamanio = config_get_int_value(config_archivo,"TAMANIO_ARCHIVO");
        char* nombre_archivo = info_archivo->nombre_archivo;
        printf("- nombre: %s\n", nombre_archivo);
        printf("- bloque inicial: %d\n", bloque);
        printf("- tamanio: %d\n", tamanio);
    }
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

    t_info_archivo* info_archivo = (t_info_archivo*)malloc(sizeof(t_info_archivo));
    info_archivo->nombre_archivo = nombre_archivo;
    info_archivo->config_archivo = config_md;

    list_add(lista_info_archivos, info_archivo);

    config_save(config_md);
    fclose(archivo_metadata);
    free(path_archivo_metadata);
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

t_info_archivo* obtener_info_archivo(char* nombre_archivo_buscado) {
    int i = 0;
    t_info_archivo* info_archivo;
    do {
        info_archivo = list_get(lista_info_archivos, i);
        i++;
    } while(strcmp(info_archivo->nombre_archivo, nombre_archivo_buscado) != 0);
    
    return info_archivo;
}

void fs_truncar_archivo(char* nombre_archivo, uint32_t nuevo_tamanio) {

    int tamanio_bloque = config_IO_DIALFS.block_size;
    //int cantidad_bloques_FS = config_IO_DIALFS.block_count;
    t_info_archivo* info_archivo = obtener_info_archivo(nombre_archivo);
    t_config* config_md = info_archivo->config_archivo;
    uint32_t bloque_inicial = config_get_int_value(config_md, "BLOQUE_INICIAL");
    uint32_t tamanio_actual = config_get_int_value(config_md, "TAMANIO_ARCHIVO");

    uint32_t cantidad_bloques_actual;
    if (tamanio_actual == 0) 
        cantidad_bloques_actual = 1;
    else {
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

    uint32_t nuevo_bloque_inicial;

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
    else if ((nuevo_bloque_inicial = buscar_primer_lugar_libre(cantidad_bloques_nueva)) != -1) {
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
        log_debug(logger_IO,"Truncar. Caso 6: Compactar y mover para ampliar");
        // leer el contenido del archivo
        FILE* arch_bloques = fopen("/home/utnso/dialfs/bloques.dat", "r+b"); // El puntero se posiciona al inicio
        char* buffer = (char*)malloc(tamanio_actual);
        fseek(arch_bloques, bloque_inicial * tamanio_bloque, SEEK_SET);
        fread(buffer, tamanio_actual, 1, arch_bloques);
        fclose(arch_bloques);

        // liberar bloques actuales
        for(int i = bloque_inicial; i <= ultimo_bloque_actual; i++) {
            bitarray_clean_bit(bitarray,i); // lo marco libre
        }

        // eliminar la info del archivo de la lista
        list_remove_element(lista_info_archivos, info_archivo);

        // compactar sin tener en cuenta el archivo a truncar
        compactar_FS();

        // buscar nueva posicion para el bloque inicial
        nuevo_bloque_inicial = buscar_primer_lugar_libre(cantidad_bloques_nueva);

        // cambiar tamanio y bloque inicial
        config_set_value(config_md, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
        config_set_value(config_md, "BLOQUE_INICIAL", string_itoa(nuevo_bloque_inicial));
        config_save(config_md);

        // actualizar info del archivo
        info_archivo->config_archivo = config_md;
        list_add(lista_info_archivos, info_archivo);

        // escribir el contenido del archivo en el nuevo lugar
        arch_bloques = fopen("/home/utnso/dialfs/bloques.dat", "r+b"); // El puntero se posiciona al inicio
        fseek(arch_bloques, nuevo_bloque_inicial * tamanio_bloque, SEEK_SET);
        fwrite(buffer, tamanio_actual, 1, arch_bloques);
        fclose(arch_bloques);
        free(buffer);

        ultimo_bloque_nuevo = nuevo_bloque_inicial + cantidad_bloques_nueva - 1;

        // asignar bloques nuevos
        for(int i = nuevo_bloque_inicial; i <= ultimo_bloque_nuevo; i++) {
            bitarray_set_bit(bitarray,i); // lo marco lleno
        }
        actualizar_bitmap();
    }

    // FIN AMPLIAR ARCHIVO
}

void compactar_FS() {
    log_info(logger_IO, "PID: <PID> - Inicio Compactación."); // LOG OBLIGATORIO
    // global: lista_info_archivos
    uint32_t tamanio_bloque = config_IO_DIALFS.block_size;

    for(int i = 0; i < list_size(lista_info_archivos); i++){
        
        t_info_archivo* info_archivo = list_get(lista_info_archivos,i);
        t_config* config_archivo = info_archivo->config_archivo;
        uint32_t bloque_inicial = config_get_int_value(config_archivo, "BLOQUE_INICIAL");
        uint32_t tamanio_archivo = config_get_int_value(config_archivo, "TAMANIO_ARCHIVO");

        uint32_t cantidad_bloques_actual;
        if (tamanio_archivo == 0) 
            cantidad_bloques_actual = 1;
        else 
            cantidad_bloques_actual = ceil((double)tamanio_archivo / (double)tamanio_bloque);

        uint32_t ultimo_bloque_actual = bloque_inicial + cantidad_bloques_actual - 1;

        // leo el contenido del archivo
        FILE* arch_bloques = fopen("/home/utnso/dialfs/bloques.dat", "r+b"); // El puntero se posiciona al inicio
        char* buffer = (char*)malloc(tamanio_archivo);
        fseek(arch_bloques, bloque_inicial * tamanio_bloque, SEEK_SET);
        fread(buffer, tamanio_archivo, 1, arch_bloques);

        // libero bloques actuales
        for(int i = bloque_inicial; i <= ultimo_bloque_actual; i++) {
            bitarray_clean_bit(bitarray,i);
        }

        // busco nueva posicion para el bloque inicial
        uint32_t nuevo_bloque_inicial = buscar_primer_lugar_libre(cantidad_bloques_actual);

        // actualizo el config con el nuevo bloque inicial
        config_set_value(config_archivo, "BLOQUE_INICIAL", string_itoa(nuevo_bloque_inicial));
        config_save(config_archivo);

        // escribo el contenido del archivo en el nuevo lugar
        fseek(arch_bloques, nuevo_bloque_inicial * tamanio_bloque, SEEK_SET);
        fwrite(buffer, tamanio_archivo, 1, arch_bloques);
        fclose(arch_bloques);
        free(buffer);

        // calculo el nuevo ultimo bloque
        uint32_t nuevo_ultimo_bloque = nuevo_bloque_inicial + cantidad_bloques_actual - 1;

        // asigno bloques nuevos
        for(int i = nuevo_bloque_inicial; i <= nuevo_ultimo_bloque; i++) {
            bitarray_set_bit(bitarray,i); // lo marco lleno
        }
        actualizar_bitmap();
    }
    log_debug(logger_IO, "esperando %.1f segundos para terminar la compactacion", (double)config_IO_DIALFS.retraso_compactacion / 1000);
    usleep(config_IO_DIALFS.retraso_compactacion * 1000); // retraso en la compactacion
    log_info(logger_IO, "PID: <PID> - Fin Compactación."); // LOG OBLIGATORIO
}

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

uint32_t buscar_primer_lugar_libre(uint32_t cantidad_bloques_nueva) {
    
    uint32_t cantidad_bloques_libres_contiguos = 0;
    uint32_t i = 0;
    uint32_t pos_primer_bloque_contiguo_libre;

    while ( (i <= config_IO_DIALFS.block_count - 1) && (cantidad_bloques_libres_contiguos != cantidad_bloques_nueva) ) {

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
        return -1;
}

void fs_eliminar_archivo(char* nombre_archivo) {
    int tamanio_bloque = config_IO_DIALFS.block_size;
    t_info_archivo* info_archivo = obtener_info_archivo(nombre_archivo);
    t_config* config_md = info_archivo->config_archivo;
    uint32_t bloque_inicial = config_get_int_value(config_md, "BLOQUE_INICIAL");
    uint32_t tamanio_actual = config_get_int_value(config_md, "TAMANIO_ARCHIVO");

    uint32_t cantidad_bloques_actual;
    if (tamanio_actual == 0) 
        cantidad_bloques_actual = 1;
    else {
        cantidad_bloques_actual = ceil((double)tamanio_actual / (double)tamanio_bloque);
    }
    uint32_t ultimo_bloque_actual = bloque_inicial + cantidad_bloques_actual - 1;
    
    // libero los bloques
    for(int i = bloque_inicial; i <= ultimo_bloque_actual; i++) {
        bitarray_clean_bit(bitarray,i); // lo marco libre
    }

    // lo elimino de la lista de archivos
    list_remove_element(lista_info_archivos, info_archivo);

    config_destroy(info_archivo->config_archivo);
    //free(info_archivo->nombre_archivo);
    free(info_archivo);
}

char* fs_leer_archivo(char* nombre_archivo, uint32_t puntero_archivo, uint32_t tamanio_a_leer) {
    uint32_t tamanio_bloque = config_IO_DIALFS.block_size;
    t_info_archivo* info_archivo = obtener_info_archivo(nombre_archivo);
    t_config* config_archivo = info_archivo->config_archivo;
    uint32_t bloque_inicial = config_get_int_value(config_archivo, "BLOQUE_INICIAL");
    //uint32_t tamanio_archivo = config_get_int_value(config_archivo, "TAMANIO_ARCHIVO");
    
    char* buffer_leido = (char*)malloc(tamanio_a_leer);
    FILE* arch_bloques = fopen("/home/utnso/dialfs/bloques.dat", "rb"); // solo lectura, puntero al inicio
    fseek(arch_bloques, bloque_inicial * tamanio_bloque + puntero_archivo, SEEK_SET); // muevo el puntero
    mostrar_info_archivos();
    fread(buffer_leido, tamanio_a_leer, 1, arch_bloques); // ACA PASA ALGO
    mostrar_info_archivos();
    fclose(arch_bloques);
    return buffer_leido;
}

void fs_escribir_archivo(char* nombre_archivo, uint32_t puntero_archivo, uint32_t tamanio_a_escribir, char* info_a_escribir) {
    uint32_t tamanio_bloque = config_IO_DIALFS.block_size;
    t_info_archivo* info_archivo = obtener_info_archivo(nombre_archivo);
    t_config* config_archivo = info_archivo->config_archivo;
    uint32_t bloque_inicial = config_get_int_value(config_archivo, "BLOQUE_INICIAL");
    //uint32_t tamanio_archivo = config_get_int_value(config_archivo, "TAMANIO_ARCHIVO");

    FILE* arch_bloques = fopen("/home/utnso/dialfs/bloques.dat", "r+b"); // lectura y escritura, puntero al inicio
    fseek(arch_bloques, bloque_inicial * tamanio_bloque + puntero_archivo, SEEK_SET); // muevo el puntero
    fwrite(info_a_escribir, tamanio_a_escribir, 1, arch_bloques);
    fclose(arch_bloques);
}