
/*
 ============================================================================
 Name        : MDJ.c
 Author      : Quantum
 Version     :
 Copyright   : Sistemas Operativos 2018
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "MDJ.h"



int i;
// char buffer[MAX_INPUT_BUFFER];							/* Buffer para leer de los socket */
 char *buffer=NULL;				/* Buffer para leer de los socket */
 int numeroClientes = 0;
 char* buffer_input_keyboard=NULL; //linea de entrada de teclado

 char* path_seleccionado[MAX_INPUT_BUFFER];
pthread_attr_t hilo_escucha;
pthread_attr_t hilo_consola_fifa;


////variables para seteo de bloques
//bloqueActual_file=NULL;
//bitmap_path_directorio=NULL;
//bitmap_file=NULL;
//bitarray=NULL;
int main(void) {
//carga de configuracion
cargar_configuracion_mdj();
mostrar_configuracion_mdj();
cargar_configuracion_metadata();
mostrar_configuracion_metadata();
char bits[metadata.cantidad_bloques/8];
cargar_configuracion_bitmap(bits,metadata.cantidad_bloques/8);
//fin de carga de config

setear_bloque_ocupado_en_posicion(62);

mostrar_bitarray();
puts("getBloquesString");
crearArchivo("checkpoint.escriptorio",299);
free(bitarray_);
puts("fin");

uint8_t byte ;
	return 0;
}

//BITMAP begin
void cargar_configuracion_bitmap(char bitmap_array[], int cantidadDeBytes){
//	static char  bitmap_array[cantidadDeBytes];
	for(int k =0;k<cantidadDeBytes;k++)bitmap_array[k]=0b00000000;
//	char* bitmap_string=malloc(cantidadDeBytes);
//	memset(bitmap_string,'0',cantidadDeBytes);//mejor con array
	printf("strlen es  %d con valor %s \n",cantidadDeBytes,bitmap_array);
	bitarray_ = bitarray_create_with_mode(bitmap_array, cantidadDeBytes, LSB_FIRST);
	bitmap_file=fopen("Bitmap.bin","w+r");
	txt_write_in_file(bitmap_file,bitarray_->bitarray);//hacerlo con mmap()
//	memmove(bitmap_file,bitarray_->bitarray,bitarray_->size);
	txt_close_file(bitmap_file);
}

void setear_bloque_ocupado_en_posicion(off_t pos){//ok
	bitarray_set_bit(bitarray_,pos);
}
void setear_bloque_libre_en_posicion(off_t pos){//ok
	bitarray_clean_bit(bitarray_,pos);
}
bool testear_bloque_libre_en_posicion(int pos){
	return bitarray_test_bit(bitarray_,(off_t)(pos));
}
void mostrar_bitarray(){
	for(int k =0;k<(bitarray_get_max_bit(bitarray_));k++)printf("test bit posicion, despues de seteo %d en pos %d \n", bitarray_test_bit(bitarray_,k),k);
}
void setBloqueActuaLleno(){//agregar un 1 al bitmap.bin
	bitarray_set_bit(bitarray_,bloqueActual_int);
}
void getBloqueLibre_path(){
	int j=getBloqueLibre_int();
	sprintf(bloqueActual_path,"%d.bin",j);//rehacer path con punto de ontaje y carpeta segun dam
	FILE* f_aux= fopen(bloqueActual_path,"w+r");//txt_open_for_append(path_bloque); SI LO ABRO COMO "W" SE BORRA EL CONTENIDO
	txt_close_file(f_aux);
//	return bloqueActual_path;
}
bool estaLibreElBloqueActual(FILE* bloqueActual, int tamanioDeBloque){
	return cantidadDeCaracteres_file(bloqueActual)<tamanioDeBloque;
}
//BITMAP end


//INTERFAZ MDJ
bool validarArchivo(char* pathDelArchivo){//ver si existe el archivo, OK, se puede borrar todos los printf() y puts(),era para probar
  	puts("---------validacion de archivo-------");
	int contador_bloques_aux=0;
  	int cantidadDeBloques=0,bytesOcupados=0;
  		t_config* aux=config_create(pathDelArchivo);
  		if (aux==NULL) {
  			perror("->validarArchivo() ,no existe el archivo o path incorrecto");
  			return false;
		}
  		bytesOcupados=config_get_int_value(aux,"TAMANIO");
  		cantidadDeBloques=(bytesOcupados/metadata.tamanio_de_bloque);
  		if((bytesOcupados%metadata.tamanio_de_bloque)!=0)cantidadDeBloques++;//esto es importante ,es como saber el numero de paginas ocupadasb
  		char** bloques_aux= config_get_array_value(aux,"BLOQUES");
  		printf("cantidad de bloques  : %d \n",cantidadDeBloques);
  		printf("metadata tamanio del bloque : %d \n",metadata.tamanio_de_bloque);
  		printf("metadata tamanio del bloque : %d \n",metadata.tamanio_de_bloque);
  		printf("bytes ocupados :  %d \n",config_get_int_value(aux,"TAMANIO"));
  		puts("inicio ciclo for ");
  		for (int var = 0; var < cantidadDeBloques; var++) {
  			printf(" bloque %s iesimo ocupado ?\n", (bloques_aux[var]));
  			if(testear_bloque_libre_en_posicion(atoi(bloques_aux[var])))contador_bloques_aux++;
  		}
  		puts("despues d ciclo for ");
			printf("contador: %d y cantidad:  %d \n", contador_bloques_aux,cantidadDeBloques);

  		free(bloques_aux);
  		config_destroy(aux);
  		puts("-------------fin validacion de archivo-------------------");
	return contador_bloques_aux==cantidadDeBloques;
}
void obtener_datos(char* pathDelArchivo,int offset, int size){
	if(validarArchivo(pathDelArchivo))fprintf(stderr,"->obtener_datos() no se puede validar path : %s",pathDelArchivo);
	else{

	}
}
void crearArchivo(char* pathDelArchivo,int cantidadDeBytesDelArchivo){//OK
	puts("---------crearArchivo()-------------------");
	int bloques_a_ocupar=((cantidadDeBytesDelArchivo)/metadata.tamanio_de_bloque);
	if((cantidadDeBytesDelArchivo%metadata.tamanio_de_bloque)!=0)bloques_a_ocupar++;
	if(getCantidadDeBloquesLibres()<bloques_a_ocupar || bloques_a_ocupar<0)perror("No se puede crearArchivo(), espacio o bloques insuficientes");
	else{
		crearBloques(bloques_a_ocupar);
		t_config *aux = config_create(pathDelArchivo);
		if(aux==NULL)perror("error en crearArchivo(), en config");
		printf("bloques a ocupar :%d \n", bloques_a_ocupar);
		printf("TAMANIO :%d \n", cantidadDeBytesDelArchivo);
		char* s2 = getBloquesLibres_string(bloques_a_ocupar);
		printf("BLOQUES= %s \n",s2);
		config_set_value(aux,"BLOQUES",s2);
		config_set_value(aux,"TAMANIO",intToString(cantidadDeBytesDelArchivo));//preguntar  el tercer parametro
		config_save_in_file(aux,pathDelArchivo);
		config_destroy(aux);
		free(s2);
		puts("fin crearArchivo()");
	}
	puts("--------------fin crearArchivo()-----------");
}
char* getBloquesLibres_string(int cantidadDeBloques){//OK,solo para crearArchivo ,da en formato "[1,2,3,54,56,6]"
//	char* bloques_string=(char*)malloc(cantidadDeBloques*sizeof(char)*3);
	char* aux=strdup("[");
	t_list *bloques_libres_aux=getBloquesLibres_list(cantidadDeBloques);
	aux=str_concat(aux,intToString((int)list_get(bloques_libres_aux,0)));
	if(cantidadDeBloques==1){
			aux=str_concat(aux,"]");
			list_destroy(bloques_libres_aux);
			return aux;
	}
	for(int var = 1; var < cantidadDeBloques; var++) {
			aux=str_concat(aux,",");
			aux=str_concat(aux,intToString((int)list_get(bloques_libres_aux,var)));
	}
		list_destroy(bloques_libres_aux);
		aux=str_concat(aux,"]");
	return aux;
}
int getBloqueLibre_int(){//obtiene el proximo bloque libre ,OK
	int j;
	for( j =0;testear_bloque_libre_en_posicion(j);j++);//hasta un bloque lbre,OK
	setear_bloque_ocupado_en_posicion(j);
	return j;
}
t_list* getBloquesLibres_list(int cantidadDeBloques){
	t_list *lista=list_create();
	for(int p=0;p<cantidadDeBloques;p++ ){
		list_add(lista,getBloqueLibre_int());
	}
	return lista;
}
int getCantidadDeBloquesLibres(){//ok
	int aux,  contador=0;
	for(aux=0;aux<metadata.cantidad_bloques;aux++){
		if(testear_bloque_libre_en_posicion(aux)==0)contador++;
	}
	return contador;
}


void guardar_datos(char* pathDelArchivo,int offset,int size, char* buffer){
	if(validarArchivo(pathDelArchivo))fprintf(stderr,"error en validar path : %s",pathDelArchivo);
	else {

	}
}

void borrarArchivo(char* pathDelArchivo){//ok
	puts("----------------borrarArchivo()---------");
	int cantidadDeBloques=0,bytesOcupados=0;
	t_config* aux=config_create(pathDelArchivo);
	if(aux==NULL)perror("->borrarArchivo() , no existe el archivo o path incorrecto");
	bytesOcupados=config_get_int_value(aux,"TAMANIO");
	cantidadDeBloques=(bytesOcupados/metadata.tamanio_de_bloque)+1;
	printf(" cantidad de bloques a borrar: %d \n",cantidadDeBloques);
	char** bloques_aux= config_get_array_value(aux,"BLOQUES");
	for(int var=0;var<cantidadDeBloques;var++){
		setear_bloque_libre_en_posicion(atoi(bloques_aux[var]));
		remove(pathDelArchivo);
	}
	free(bloques_aux);
	config_destroy(aux);
	puts("-------------fin de borrarArchivo()----------");
}

//INTERFAZ MDJ


void consola_fifa(){
	puts("press \"exit\" para salir de consola ");
	loop{
			buffer_input_keyboard=readline("fifa@mdj=>  ");
			if(!strncmp(buffer_input_keyboard, "exit", 4)) break;
			ejecutar_linea_entrante(buffer_input_keyboard);
			free(buffer_input_keyboard);
	}
}
void  ejecutar_linea_entrante(char* buffer_entrante){
//	system(buffer_entrante);
	persistirContenido(buffer_entrante);
}
void persistirContenido(char * contenido){
	 bloqueActual_path=malloc(400);
	for(getBloqueLibre_path();quedaContenidoParaMapear(contenido);getBloqueLibre_path()){
			persistirAlBloque(bloqueActual_path,contenido);
			if(estaLLenoElBloqueActual())setear_bloque_ocupado_en_posicion(bloqueActual_int);
		}
			setear_bloque_ocupado_en_posicion(bloqueActual_int);
	free(bloqueActual_path);
}


 int  espacioRestanteAlBloque(char* pathDelBloque){
	int resto =  metadata.tamanio_de_bloque-cantidadDeCaracteres_path(pathDelBloque);
//	if (resto<0) { // no deberia de pasar esto
//		setear_bloque_ocupado_en_posicion(bloqueActual_int);
//	}
	return resto;
}


void  persistirAlBloque(char* unBloquePath, char * contenido){
	char* recorte =NULL;
	recorte=recortarPrimerosCaracteres(contenido,espacioRestanteAlBloque(unBloquePath));
	printf("se va a mapear al bloque %s y el restante es %s \n",recorte,contenido);
//	 sprintf(bloqueActual_path,"%d.bin",bloqueActual_int);

	FILE* bloqueActual =txt_open_for_append(unBloquePath);
	txt_write_in_file(bloqueActual,recorte);
	txt_close_file(bloqueActual);
	free(recorte);
}
bool estaLLenoElBloqueActual(){
	return cantidadDeCaracteres_path(bloqueActual_path)==metadata.tamanio_de_bloque;
}

bool terminoDeMapearContenido(){//en revision
	bool hayCaracteresParaMapear=strlen(buffer_input_keyboard)>0;
	return bitarray_test_bit(bitarray_,bloqueActual_int)&&hayCaracteresParaMapear;
}


void mostrar_configuracion_mdj(){
 	printf("iniciando lectura de configuracion...\n");
 	printf("PUNTO_DE_MONTAJE = %s \n",mdj.punto_de_montaje);
 	printf("RETARDO = %d \n",mdj.retardo);
 	printf("PUERTO MDJ = %s \n",mdj.puerto);
 	printf("IP MDJ = %s \n",mdj.ip);
 	printf("---fin lectura de configuracion --- .. \n\n");
 }

void mdj_liberar_recursos(){
	 loggear_y_guardar_info("MDJ terminando..");
	 log_info(mdj_log, "Finish.cfg");
	 log_destroy(mdj_log);
	 free(buffer);
}



void inicializando_socket(){
	mdj_socket=crear_socket(mdj.ip,mdj.puerto);
		 puts("conectando socket");
		 socketServidor = mdj_socket.socket;
		 	//Asocio el servidor a un puerto
		 	asociar_puerto(mdj_socket);
		 	//Escucho Conexiones Entrantes
		 	escuchar(mdj_socket);
}

void escuchar_mensajes_entrantes(){
	inicializando_socket();
	while (1)
		 		{
		 			/* Cuando un cliente cierre la conexión, se pondrá un -1 en su descriptor
		 			 * de socket dentro del array socketCliente. La función compactaClaves()
		 			 * eliminará dichos -1 de la tabla, haciéndola más pequeña.
		 			 *
		 			 * Se eliminan todos los clientes que hayan cerrado la conexión */
		 			compactaClaves (socketCliente, &numeroClientes);

		 			/* Se inicializa descriptoresLectura */
		 			FD_ZERO (&descriptoresLectura);

		 			/* Se añade para select() el socket servidor */
		 			FD_SET (socketServidor, &descriptoresLectura);

		 			/* Se añaden para select() los sockets con los clientes ya conectados */
		 			for (i=0; i<numeroClientes; i++)
		 				FD_SET (socketCliente[i], &descriptoresLectura);

		 			/* Se el valor del descriptor más grande. Si no hay ningún cliente,
		 			 * devolverá 0 */
		 			maximo = dameMaximo (socketCliente, numeroClientes);

		 			if (maximo < socketServidor)
		 				maximo = socketServidor;

		 			/* Espera indefinida hasta que alguno de los descriptores tenga algo
		 			 * que decir: un nuevo cliente o un cliente ya conectado que envía un
		 			 * mensaje */
		 			select(maximo + 1, &descriptoresLectura, NULL, NULL, NULL);

		 			/* Se comprueba si algún cliente ya conectado ha enviado algo */
		 			for (i=0; i<numeroClientes; i++)
		 			{
		 				if (FD_ISSET (socketCliente[i], &descriptoresLectura))
		 				{
		 					/* Se lee lo enviado por el cliente y se escribe en pantalla */
//		 					int bytes_recibidos=Lee_Socket(  socketCliente[i], (char *)&buffer, sizeof(buffer));
		 					int recibi_caracteres= Lee_Socket(  socketCliente[i],buffer, MAX_INPUT_BUFFER);
		 					int cant_caracteres_recibidos=strlen(buffer);
		 					if (recibi_caracteres){
		 						loggear_y_guardar_info("Cliente %d envía %s \n", i+1, buffer);
		 						loggear_y_guardar_info("cant letras recibidos => %d \n ",cant_caracteres_recibidos);
//		 						mostrar_y_guardar_log("cant caracteres recibidos => %d \n ",cant_caracteres_recibidos);

		 					}
		 					else
		 					{
		 						/* Se indica que el cliente ha cerrado la conexión y se
		 						 * marca con -1 el descriptor para que compactaClaves() lo
		 						 * elimine */
		 						loggear_info("Cliente %d ha cerrado la conexión \n", i+1);
//		 						perror(leyenda_temporal);
		 						 goto salto2;
		 						socketCliente[i] = -1;
		 						pthread_exit(NULL);//ver si corta el hilo
//		 						return 0;
		 					}
		 				}
		 			}

		 			/* Se comprueba si algún cliente nuevo desea conectarse y se le
		 			 * admite */
		 			if (FD_ISSET (socketServidor, &descriptoresLectura))
		 				nuevoCliente (socketServidor, socketCliente, &numeroClientes);
		 		}

	salto2://puts("");
	cerrar_socket(mdj_socket);
}




//METADATA
void cargar_configuracion_metadata(){//tamaniohardcodeado para probar
	t_config *configuracion_cfg_temporal=cargar_en_memoria_cfg("Metadata.bin");
	if(configuracion_cfg_temporal==NULL)perror("no cargo bien  Metadata.bin\n");
//	(&metadata)->cantidad_bloques=config_get_int_value(configuracion_cfg_temporal,"CANTIDAD_BLOQUES");
	metadata.cantidad_bloques=config_get_int_value(configuracion_cfg_temporal,"CANTIDAD_BLOQUES");
	metadata.tamanio_de_bloque=config_get_int_value(configuracion_cfg_temporal,"TAMANIO_BLOQUES");
	config_destroy(configuracion_cfg_temporal);
}
void mostrar_configuracion_metadata(){
	puts("leyendo metadata");
	printf("tamanio bloque %d \n", metadata.tamanio_de_bloque);
	printf("cantidad_bloques %d \n", metadata.cantidad_bloques);
	puts("fin lectura metadata ");
}
//METADATA






