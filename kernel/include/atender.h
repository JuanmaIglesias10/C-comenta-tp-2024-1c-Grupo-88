#include <kernel.h>

void* atender_memoria(int cliente_fd);
void* atender_IO(int cliente_fd);
void* atender_cpu_int(int cliente_fd);
void* atender_cpu_dis(int cliente_fd);
