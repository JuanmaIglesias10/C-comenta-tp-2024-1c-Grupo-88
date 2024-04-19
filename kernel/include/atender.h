#include "../../utils/include/utils.h"

void* atender_memoria(t_log *logger, int cliente_fd);
void* atender_IO(t_log *logger, int cliente_fd);
void* atender_cpu_int(t_log *logger, int cliente_fd);
void* atender_cpu_dis(t_log *logger, int cliente_fd);
