#ifndef CPU_H_
#define CPU_H_


#include <commons/string.h> //Commons string
#include <qcommons/console.h>
#include <qcommons/socket.h> //Libreria Socket Cliente
#include <qcommons/protocolos.h>
#include <signal.h>
#include <stdbool.h>

#include "config/config.h"//Llamadas para leer y mostrar el archivo config
#include "parser/parser.h"//parser del Lenguaje EscripTorio


#define FILE_CONFIG_CPU "cpu.config"
#define EXTENSION_FILE_LOG_CPU ".log"
#define CPU "CPU"

int socket_safa, socket_diego, socket_fm9;
struct_instruccion instruccion;




//Funciones
void conectarse_con_safa();
void conectarse_con_diego();
void conectarse_con_fm9();
int inicializar(char* nombre_archivo_log);
char* crear_nombre_file_log();
void liberar_recursos(int tipo_salida);
void exit_gracefully();

void ejecutar_instruccion(struct_instruccion instruccion);
void liberar_instruccion(struct_instruccion instruccion);


//////EJECUCION DE OPERACIONES//////
unsigned escriptorio_abrir(char** parametros);
unsigned escriptorio_concentrar(char** parametros);
unsigned escriptorio_asignar(char** parametros);
unsigned escriptorio_wait(char** parametros);
unsigned escriptorio_signal(char** parametros);
unsigned escriptorio_flush(char** parametros);
unsigned escriptorio_close(char** parametros);
unsigned escriptorio_crear(char** parametros);
unsigned escriptorio_borrar(char** parametros);
unsigned escriptorio_comentario(char** parametros);
///////////////////////////////////

bool se_encuentra_archivo_en_gdt(path);





#endif /* CPU_H_ */
