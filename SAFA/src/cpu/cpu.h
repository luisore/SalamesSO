#ifndef CPU_CPU_H_
#define CPU_CPU_H_

#include "../commons/commons.h"
#include "../config/config.h"

t_list *dtb_nuevos;
t_list *dtb_listos;
t_list *dtb_bloqueados;
t_list *dtb_terminados;
t_list *dtbs;

dtb_struct *dtb_ejecutando;

void inicializar_listas_cpu();
dtb_struct crear_dtb(char *path);
cpu_struct* crear_cpu(int socket);
dtb_struct *obtener_proximo_dtb(char *algoritmo);
cpu_struct *obtener_cpu_libre();
void liberar_cpu(cpu_struct * cpu);
void liberar_recursos_cpu();


#endif /* CPU_CPU_H_ */
