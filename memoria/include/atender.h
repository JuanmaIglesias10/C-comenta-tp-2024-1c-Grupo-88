#include "../../utils/include/utils.h"
void* atender_cpu(t_log *logger, int cliente_fd);
void* atender_IO(t_log *logger, int cliente_fd);
void* atender_kernel(t_log *logger, int cliente_fd);