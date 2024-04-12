#include <stdlib.h>
#include <stdio.h>
#include <../../utils/include/protocolo.h>
#include <../../utils/include/sockets.h>

t_config* iniciar_config(void);
t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level)