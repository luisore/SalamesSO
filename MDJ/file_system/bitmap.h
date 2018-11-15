/*
 * bitmap.h
 *
 *  Created on: 15 nov. 2018
 *      Author: jhondaniel
 */

#ifndef FILE_SYSTEM_BITMAP_H_
#define FILE_SYSTEM_BITMAP_H_
#include <string.h>
#include <stdio.h>
#include <commons/bitarray.h>
#include <commons/txt.h>
#include "util.h"

void configurar_bitmap();
void setear_bloque_ocupado_en_posicion(int pos);
bool testear_bloque_libre_en_posicion(int pos);
void setBloqueLleno();//agregar un 1 al bitmap.bin
void mostrar_bitarray(int cantidadDeBloques);


FILE* getBloqueLibre_file();
bool estaLibreElBloqueActual(FILE* bloqueActual, int tamanioDeBloque);


#endif /* FILE_SYSTEM_BITMAP_H_ */