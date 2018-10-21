#ifndef DTB_DTB_H_
#define DTB_DTB_H_

#include "../commons/commons.h"
#include "../config/config.h"


t_list *dtb_nuevos;
t_list *dtb_listos;
t_list *dtb_ejecutando;
t_list *dtb_bloqueados;
t_list *dtb_terminados;
t_list *dtbs;

void inicializar_listas_dtb();
dtb_struct crear_dtb(int id_dtb, char *path);
dtb_struct *obtener_proximo_dtb(int algoritmo);
void liberar_dtb(dtb_struct * dtb);
void liberar_recursos_dtb();


#endif /* DTB_DTB_H_ */
