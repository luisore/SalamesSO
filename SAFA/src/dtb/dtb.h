#ifndef DTB_DTB_H_
#define DTB_DTB_H_

#include "../commons/commons.h"
#include "../config/config.h"




void inicializar_listas_dtb();
dtb_struct crear_dtb(int id_dtb, char *path);
void liberar_dtb(dtb_struct * dtb);
void liberar_recursos_dtb();
void inicializar_dummy(dtb_struct* dtb);
void reiniciar_dummy(); //REINICIA DTB DUMMY PARA QUE PUEDA SER INICIALIZADO POR OTRO DTB


dtb_struct *obtener_dtb_a_ejecutar_dummy(); //funcion que me devuelve el primer dtb de la lista de nuevos en estado nuevo
dtb_struct *obtener_dtb_a_listos(); //funcion que me devuelve un dtb para pasar a listos. Ya sea en estado Nuevo o CargaLista
bool dtb_estado_nuevo_cargado(dtb_struct *dtb); 	// indica si dtb esta en estado nuevo o cargado .

void agregar_direccion_a_dtb( dtb_struct *dtb , direccion_struct *direccion );  //agrego la direccion al dtb
void eliminar_path_dtb( dtb_struct *dtb   , char* path ); //elimina path de la lista de direcciones.


void aumentar_sentencias_totales( dtb_struct *dtb , int quantum ); //aumento las sentencias totales de un dtb
void aumentar_sentencias_espera( int quantum_ejecutado ); //aumento las sentencias en espera de todos los dtbs que se encuentran en la lista de nuevos


#endif /* DTB_DTB_H_ */
