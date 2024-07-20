#include "manejoIO.h"

void io_gen_sleep(char* interfaz, char* char_unidadesDeTrabajo) {
    uint8_t unidadesDeTrabajo = atoi(char_unidadesDeTrabajo);

    sem_wait(&sem_colaGEN);

    pthread_mutex_lock(&mutex_colaGEN);
    t_interfaz* aux = queue_pop(colaGenerica);
    pthread_mutex_unlock(&mutex_colaGEN);
    
    // Chequeo si aux es NULL
    if (aux != NULL) {
        if (strcmp(aux->nombre, interfaz) == 0 ){
            if (strcmp(aux->tipo , "GENERICA") == 0) {
                enviar_codOp(aux->fd, SLEEP);
                t_buffer* buffer_a_enviar = crear_buffer();
                agregar_buffer_uint8(buffer_a_enviar, unidadesDeTrabajo);
                enviar_buffer(buffer_a_enviar, aux->fd);
                destruir_buffer(buffer_a_enviar);
                
                // Bloqueo el proceso
                t_pcb* pcb_sleep = malloc(sizeof(t_pcb)); 
                pcb_sleep = pcb_ejecutando;
                             
                enviar_de_exec_a_block();
                
                mensajeKernelIO codigo = recibir_codOp(aux->fd);
                if (codigo == SLEEP_OK) {
                    if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_sleep->quantum) {
                        pcb_sleep->quantum -= ms_transcurridos;
                        queue_push(colaGenerica, aux);
                        sem_post(&sem_colaGEN);
                        enviar_pcb_de_block_a_ready_mas(pcb_sleep);
                    } else {
                        queue_push(colaGenerica, aux);
                        sem_post(&sem_colaGEN);
                        enviar_pcb_de_block_a_ready(pcb_sleep);
                    }
                }
            } else {
                agregar_a_cola_finished("INVALID_INTERFACE");
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }
    // free(interfaz);
}


void* io_stdin_read(void* arg) {
    parametros_read_t* params = (parametros_read_t*)arg;
    uint32_t direccion_fisica = atoi(params->char_direccion_fisica);
    uint32_t tamanio = atoi(params->char_tamanio);
    
    //t_pcb* pcb_aux = malloc(sizeof(t_pcb));
    t_pcb* pcb_aux = pcb_ejecutando;
    enviar_de_exec_a_block();
    
    sem_wait(&sem_colaREAD);
    t_interfaz* aux = queue_pop(colaSTDIN);
    
    if (strcmp(aux->nombre, params->interfaz) == 0) {
        if (strcmp(aux->tipo, "STDIN") == 0) {
            enviar_codOp(aux->fd, STDIN_READ);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, direccion_fisica);
            agregar_buffer_uint32(buffer_a_enviar, tamanio);
            agregar_buffer_uint32(buffer_a_enviar, pcb_aux->cde->pid);


            enviar_buffer(buffer_a_enviar, aux->fd);
            destruir_buffer(buffer_a_enviar);

            // Bloqueo el proceso

            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == STDIN_READ_OK) {
                log_info(logger_kernel, "Llegue a STDIN_READ_OK");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_aux->quantum) {
                    pcb_aux->quantum -= ms_transcurridos;
                    queue_push(colaSTDIN, aux);
                    sem_post(&sem_colaREAD);

                    enviar_pcb_de_block_a_ready_mas(pcb_aux);
                } else {
                    sem_post(&sem_colaREAD);
                    queue_push(colaSTDIN, aux);
                    enviar_pcb_de_block_a_ready(pcb_aux);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }
    free(params->interfaz);
    free(params->char_direccion_fisica);
    free(params->char_tamanio);
    free(params);
    return NULL;
}

void io_stdout_write(char* interfaz, char* char_direccion_fisica, char* char_tamanio) {
    uint32_t direccion_fisica   = atoi(char_direccion_fisica);
    uint32_t tamanio            = atoi(char_tamanio);

    //sem_wait(&sem_colaWRITE);
    t_interfaz* aux = queue_pop(colaSTDOUT);

    if (strcmp(aux->nombre, interfaz) == 0 ){
        if (strcmp(aux->tipo , "STDOUT") == 0) {
            enviar_codOp(aux->fd, STDOUT_WRITE);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, direccion_fisica);
            agregar_buffer_uint32(buffer_a_enviar, tamanio);
            agregar_buffer_uint32(buffer_a_enviar, pcb_ejecutando->cde->pid);

            enviar_buffer(buffer_a_enviar, aux->fd);

            destruir_buffer(buffer_a_enviar);

            // Bloqueo el proceso
            t_pcb* pcb_aux = pcb_ejecutando;
            enviar_de_exec_a_block();

            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == STDOUT_WRITE_OK) {
                log_info(logger_kernel, "Llegue a STDOUT_WRITE_OK");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_aux->quantum) {
                    pcb_aux->quantum -= ms_transcurridos;
                    // sem_post(&sem_colaWRITE);
                    queue_push(colaSTDOUT, aux);
                    enviar_pcb_de_block_a_ready_mas(pcb_aux);
                } else {
                    // sem_post(&sem_colaWRITE);
                    queue_push(colaSTDOUT, aux);
                    enviar_pcb_de_block_a_ready(pcb_aux);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }
}

//TODO : Es necesesario librear pcb_aux en las funciones, ver luego
void io_fs_create(char* interfaz, char* nombreArchivo){
    //sem_wait(&sem_colaFS);
    //t_pcb* pcb_aux = malloc(sizeof(t_pcb));
    t_pcb* pcb_aux = pcb_ejecutando;
    enviar_de_exec_a_block();
    t_interfaz* aux = queue_pop(colaDIALFS);

    if (strcmp(aux->nombre, interfaz) == 0 ){
        if (strcmp(aux->tipo , "DIALFS") == 0) {
            enviar_codOp(aux->fd, FS_CREATE);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, pcb_aux->cde->pid);
            agregar_buffer_string(buffer_a_enviar, nombreArchivo);


            enviar_buffer(buffer_a_enviar, aux->fd);

            destruir_buffer(buffer_a_enviar);


            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == FS_CREATE_OK) {
                log_info(logger_kernel, "Llegue a FS_CREATE ok");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_aux->quantum) {
                    pcb_aux->quantum -= ms_transcurridos;
                    enviar_pcb_de_block_a_ready_mas(pcb_aux);
                    //sem_post(&sem_colaFS);
                    queue_push(colaDIALFS, aux);
                } else {
                    //sem_post(&sem_colaFS);
                    queue_push(colaDIALFS, aux);
                    enviar_pcb_de_block_a_ready(pcb_aux);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }


}

void io_fs_delete(char* interfaz, char* nombreArchivo){
    //sem_wait(&sem_colaFS); 
    //t_pcb* pcb_aux = malloc(sizeof(t_pcb)); 
    t_pcb* pcb_aux = pcb_ejecutando;
    enviar_de_exec_a_block();
    t_interfaz* aux = queue_pop(colaDIALFS);

    if (strcmp(aux->nombre, interfaz) == 0 ){
        if (strcmp(aux->tipo , "DIALFS") == 0) {
            enviar_codOp(aux->fd, FS_DELETE);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, pcb_aux->cde->pid);
            agregar_buffer_string(buffer_a_enviar, nombreArchivo);


            enviar_buffer(buffer_a_enviar, aux->fd);

            destruir_buffer(buffer_a_enviar);


            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == FS_DELETE_OK) {
                log_info(logger_kernel, "Llegue a FS_DELETE_OK");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_aux->quantum) {
                    pcb_aux->quantum -= ms_transcurridos;
                    enviar_pcb_de_block_a_ready_mas(pcb_aux);
                    //sem_post(&sem_colaFS);
                    queue_push(colaDIALFS, aux);
                } else {
                    //sem_post(&sem_colaFS);
                    queue_push(colaDIALFS, aux);
                    enviar_pcb_de_block_a_ready(pcb_aux);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }


}

void* io_fs_truncate(void* arg){
    parametros_truncate_t* params = (parametros_truncate_t*)arg;
    char* nombreArchivo = params->nombreArchivo;
    uint32_t tamanio = atoi(params->tamanio);

    //t_pcb* pcb_aux = malloc(sizeof(t_pcb));
    t_pcb* pcb_aux = pcb_ejecutando;
    enviar_de_exec_a_block();
    
    // sem_wait(&sem_colaFS);
    t_interfaz* aux = queue_pop(colaDIALFS);
    
    if (strcmp(aux->nombre, params->interfaz) == 0) {
        if (strcmp(aux->tipo, "DIALFS") == 0) {
            enviar_codOp(aux->fd, FS_TRUNCATE);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, pcb_aux->cde->pid);
            agregar_buffer_string(buffer_a_enviar, nombreArchivo);
            agregar_buffer_uint32(buffer_a_enviar, tamanio);

            enviar_buffer(buffer_a_enviar, aux->fd);
            destruir_buffer(buffer_a_enviar);

            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == FS_TRUNCATE_OK) {
                log_info(logger_kernel, "Llegue a FS_TRUNCATE OK");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_aux->quantum) {
                    pcb_aux->quantum -= ms_transcurridos;
                    // sem_post(&sem_colaFS);
                    queue_push(colaDIALFS, aux);

                    enviar_pcb_de_block_a_ready_mas(pcb_aux);
                } else {
                    // sem_post(&sem_colaFS);
                    queue_push(colaDIALFS, aux);
                    enviar_pcb_de_block_a_ready(pcb_aux);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }
    free(params->interfaz);
    free(params->nombreArchivo);
    free(params->tamanio);
    free(params);
    return NULL;
}

void* io_fs_write(void* arg) {
    parametros_fs_write_read_t* params = (parametros_fs_write_read_t*)arg;
    char* nombreArchivo = params->nombreArchivo;
    uint32_t direccion_fisica = atoi(params->dir_fisica);
    uint32_t tamanio = atoi(params->tamanio);
    uint32_t valor_puntero = atoi(params->valor_puntero);

    t_pcb* pcb_aux = pcb_ejecutando;
    enviar_de_exec_a_block();
    
    // sem_wait(&sem_colaFS);
    t_interfaz* aux = queue_pop(colaDIALFS);
    
    if (strcmp(aux->nombre, params->interfaz) == 0) {
        if (strcmp(aux->tipo, "DIALFS") == 0) {
            enviar_codOp(aux->fd, FS_WRITE);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, pcb_aux->cde->pid);
            agregar_buffer_string(buffer_a_enviar, nombreArchivo);
            agregar_buffer_uint32(buffer_a_enviar, direccion_fisica);
            agregar_buffer_uint32(buffer_a_enviar, tamanio);
            agregar_buffer_uint32(buffer_a_enviar, valor_puntero);

            enviar_buffer(buffer_a_enviar, aux->fd);
            destruir_buffer(buffer_a_enviar);

            // Bloqueo el proceso

            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == FS_WRITE_OK) {
                log_info(logger_kernel, "Llegue a FS_WRITE_OK");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_aux->quantum) {
                    pcb_aux->quantum -= ms_transcurridos;
                    queue_push(colaDIALFS, aux);
                    // sem_post(&sem_colaFS);

                    enviar_pcb_de_block_a_ready_mas(pcb_aux);
                } else {
                    // sem_post(&sem_colaFS);
                    queue_push(colaDIALFS, aux);
                    enviar_pcb_de_block_a_ready(pcb_aux);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }
    
    free(params->interfaz);
    free(params->nombreArchivo);
    free(params->dir_fisica);
    free(params->tamanio);
    free(params->valor_puntero);
    free(params);
    return NULL;
}

void* io_fs_read(void* arg) {
    parametros_fs_write_read_t* params = (parametros_fs_write_read_t*)arg;
    char* nombreArchivo = params->nombreArchivo;
    uint32_t direccion_fisica = atoi(params->dir_fisica);
    uint32_t tamanio = atoi(params->tamanio);
    uint32_t valor_puntero = atoi(params->valor_puntero);

    //t_pcb* pcb_aux = malloc(sizeof(t_pcb));
    t_pcb* pcb_aux = pcb_ejecutando;
    enviar_de_exec_a_block();
    
    // sem_wait(&sem_colaFS);
    t_interfaz* aux = queue_pop(colaDIALFS);
    
    if (strcmp(aux->nombre, params->interfaz) == 0) {
        if (strcmp(aux->tipo, "DIALFS") == 0) {
            enviar_codOp(aux->fd, FS_READ);
            t_buffer* buffer_a_enviar = crear_buffer();

            agregar_buffer_uint32(buffer_a_enviar, pcb_aux->cde->pid);
            agregar_buffer_string(buffer_a_enviar, nombreArchivo);
            agregar_buffer_uint32(buffer_a_enviar, direccion_fisica);
            agregar_buffer_uint32(buffer_a_enviar, tamanio);
            agregar_buffer_uint32(buffer_a_enviar, valor_puntero);

            enviar_buffer(buffer_a_enviar, aux->fd);
            destruir_buffer(buffer_a_enviar);

            // Bloqueo el proceso

            mensajeKernelIO codigo = recibir_codOp(aux->fd);
            if (codigo == FS_READ_OK) {
                log_info(logger_kernel, "Llegue a FS_READ_OK");
                if (strcmp(config_kernel.algoritmo_planificacion, "VRR") == 0 && ms_transcurridos < pcb_aux->quantum) {
                    pcb_aux->quantum -= ms_transcurridos;
                    queue_push(colaDIALFS, aux);
                    // sem_post(&sem_colaFS);

                    enviar_pcb_de_block_a_ready_mas(pcb_aux);
                } else {
                    // sem_post(&sem_colaFS);
                    queue_push(colaDIALFS, aux);
                    enviar_pcb_de_block_a_ready(pcb_aux);
                }
            }
        } else {
            agregar_a_cola_finished("INVALID_INTERFACE");
        }
    } else {
        agregar_a_cola_finished("INVALID_INTERFACE");
    }
    free(params->interfaz);
    free(params->nombreArchivo);
    free(params->dir_fisica);
    free(params->tamanio);
    free(params->valor_puntero);
    free(params);
    return NULL;
}