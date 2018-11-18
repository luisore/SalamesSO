/*
 ============================================================================
 Name        : DAM.c
 Author      : Quantum
 Version     :
 Copyright   : Sistemas Operativos 2018
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "DAM.h"

pthread_t idHilo;

int MAX_LINEA=0;

int main(void) {
	if (inicializar() < 0) {
		liberar_recursos(EXIT_FAILURE);
		return -1;
	}

	imprimir_config();

	//Realizar Conexiones hacia SAFA, FM9, MDJ
	realizarConexiones();

	//Servidor con Hilos
	servidorDAM();

	//Aceptar Conexiones de CPU y lanzar un Hilo por cada CPU
	aceptarConexiones();

	liberar_recursos(EXIT_SUCCESS);

	return EXIT_SUCCESS;
}

int inicializar() {
	if (crear_log() == EXIT_FAILURE)
		terminar_exitosamente(EXIT_FAILURE);

	print_header(DAM_LOG, dam_log);

	if (cargar_archivo_config(FILE_CONFIG_DAM) < 0) {
		return -1;
	}

	return 0;
}

void liberar_recursos(int tipo_salida) {
	liberar_recursos_configuracion();
	print_footer(DAM_LOG, dam_log);
	destruir_archivo_log(dam_log);
	terminar_exitosamente(tipo_salida);
}

void terminar_exitosamente(int ret_val) {
	if (socket_dam != 0)
		close(socket_dam);
	exit(ret_val);
}

void servidorDAM() {
	//Servidor con Hilos
	log_info(dam_log, "Servidor con Hilos");
	if (configurar_socket_servidor(&socket_dam, "127.0.0.1", dam.puerto_dam,
			TAMANIO_CANT_CLIENTES) < 0) {
		log_error(dam_log, "No se pudo iniciar el servidor");
		liberar_recursos(EXIT_FAILURE);
	}
}

void aceptarConexiones() {
	log_info(dam_log, "Esperando por conexiones entrantes...");
	/*Por cada una de las conexiones que sean aceptadas, se lanza
	 un Hilo encargado de atender la conexión*/
	while (1) {
		int socket_cliente = aceptar_conexion(socket_dam);
		log_info(dam_log, "Se agrego una nueva conexión, socket: %d",
				socket_cliente);
		if (socket_cliente <= 0) {
			log_error(dam_log, "Error al aceptar nueva conexión");
		}
		pthread_create(&idHilo, NULL, (void*) conexion_cpu, &socket_cliente);
	}
}

void realizarConexiones() {
	conectar_safa(dam);
	//conectar_mdj(dam);
	conectar_fm9(dam);
	/*if (socket_safa <= 0 || socket_mdj <= 0 || socket_fm9 <= 0) {
		log_error(dam_log, "Error al conectarse a safa/mdj/fm9");
		liberar_recursos(EXIT_FAILURE);
	}
	else
	{
	log_info(dam_log, "Realizada Conexiones safa/mdj/fm9");
	}*/
}

void conectar_safa(dam_config dam){

	obtener_socket_cliente(&socket_safa,dam.ip_safa,dam.puerto_safa);
	log_info(dam_log,"HANDSHAKE con DAM");
	ejecutar_handshake(socket_safa,"DAM",DAM,dam_log);
}

void conectar_mdj(dam_config dam){

	obtener_socket_cliente(&socket_mdj,dam.ip_mdj,dam.puerto_mdj);
	log_info(dam_log,"HANDSHAKE con MDJ");
	//ejecutar_handshake(socket_mdj,"DAM",DAM,dam_log);
}

void conectar_fm9(dam_config dam){
	obtener_socket_cliente(&socket_fm9,dam.ip_fm9,dam.puerto_fm9);
	//ejecutar_handshake(socket_fm9,"DAM",DAM,dam_log);

	header_paquete* paquete = malloc(sizeof(header_paquete));
	paquete->tipo_instancia = DAM;
	paquete->tipo_operacion = HANDSHAKE;

	log_info(dam_log,"HANDSHAKE con FM9");
	send(socket_fm9,paquete,sizeof(header_paquete),0);
	recv(socket_fm9,&MAX_LINEA,sizeof(int),MSG_WAITALL);

	log_info(dam_log,"TAM LINEA: %d", MAX_LINEA);
}

void conexion_cpu (void *parametro) {
	int cliente_socket = *(int *) parametro;
    log_info(dam_log, "Conectado CPU");

	realizarHandshakeCpu(cliente_socket);

	atender_operacion_cpu(cliente_socket);


    //cerrar_socket(*sock);
}

void realizarHandshakeCpu(int cliente_socket) {
	header_conexion_type* header_conexion = NULL;
	mensaje_reconocimiento_type mensaje_reconocimiento;
	void* buffer_reconocimiento;
	void* buffer_header = malloc(TAMANIO_HEADER_CONEXION);
	/************ LEER EL HANDSHAKE ************/
	int res = recv(cliente_socket, buffer_header, TAMANIO_HEADER_CONEXION,
			MSG_WAITALL);
	if (res <= 0) {
		log_error(dam_log, "¡Error en el handshake con el cliente! %d", res);
		close(cliente_socket);
		free(buffer_header);
	}
	header_conexion = deserializar_header_conexion(buffer_header);
	log_info(dam_log, "Se realizo handshake del cliente: %s",
			header_conexion->nombre_instancia);
	/************ RESPONDER AL HANDSHAKE ************/
	strcpy(mensaje_reconocimiento.nombre_instancia, "DAM");
	buffer_reconocimiento = serializar_mensaje_reconocimiento(
			&mensaje_reconocimiento);
	if (send(cliente_socket, buffer_reconocimiento,
			TAMANIO_MENSAJE_RECONOCIMIENTO, 0)
			!= TAMANIO_MENSAJE_RECONOCIMIENTO) {
		log_error(dam_log, "¡No se pudo devolver el handshake al cliente!");
		close(cliente_socket);
	} else {
		log_info(dam_log, "El cliente %s se ha conectado correctamente",
				header_conexion->nombre_instancia);
	}

	free(buffer_header);
	free(header_conexion);
	free(buffer_reconocimiento);
}

void atender_operacion_cpu(int cliente_socket) {

	header_paquete* paquete = malloc(sizeof(header_paquete));
	char linea[MAX_LINEA];

	while ( (recv(cliente_socket, paquete, sizeof(header_paquete),MSG_WAITALL) )  > 0)
	{
		log_info(dam_log, "Se recibio operacion del CPU: %s",paquete->tipo_operacion);
		switch (paquete->tipo_operacion)
		{

			//solicitud a El Diego para que traiga desde el MDJ el archivo requerido
			case ABRIR:{
				void* buffer = malloc(paquete->tamanio_mensaje);
				recv(cliente_socket,&buffer,paquete->tamanio_mensaje,MSG_WAITALL);

				operacion_archivo* operacion_abrir = deserializar_operacion_archivo(buffer);

				log_info(dam_log,"/Procesando/ Abrir Archivo, PID: %s, Archivo: %s",operacion_abrir->pid,operacion_abrir->ruta_archivo);

				int* tam_buffer_mdj = malloc(sizeof(int));
				operacion_archivo_mdj* operacion_archivo_mdj = malloc(sizeof(operacion_archivo_mdj));
				operacion_archivo_mdj->ruta_archivo = string_duplicate(operacion_abrir->ruta_archivo);

				void* buffer_mdj = serializar_operacion_archivo_mdj(operacion_archivo_mdj,tam_buffer_mdj);

				log_info(dam_log,"Enviando Peticion MDJ Abrir Archivo");

				header_paquete* paquete=malloc(sizeof(header_paquete));
				paquete->tipo_operacion = OBTENER_DATOS;
				paquete->tamanio_mensaje = *tam_buffer_mdj;

				send(socket_mdj,paquete,sizeof(header_paquete),0);
				send(socket_mdj,buffer_mdj,(size_t)tam_buffer_mdj,0);

				/*CONVERSION DE BYTES A LINEAS*/
				int se_encuentra_barra_n(char* buffer_chars)
				{
					return strchr(buffer_chars,'\n') != NULL;
				}

				send(socket_mdj,(void*)&MAX_LINEA,sizeof(int),0);

				memset( linea, '\0', sizeof(char)*MAX_LINEA );
				int bytesRecibidos=0;
				int lastIndex=0;
				while(1)
				{
					void* buffer_bytes=malloc(dam.transfer_size+1);

					bytesRecibidos+=recv(socket_mdj,buffer_bytes,dam.transfer_size,0);

					log_info(dam_log,"Bytes: %d",bytesRecibidos);

					char* buffer_chars = (char*)buffer_bytes;
					buffer_chars[dam.transfer_size] = '\0';

					if(se_encuentra_barra_n(buffer_chars))
					{
						myMemCpy(&(linea[lastIndex]), buffer_chars, dam.transfer_size);
						log_info(dam_log,"LINEA COMPLETA CON BARRA_N");
						log_info(dam_log,"%s",linea);
						lastIndex=0;
						memset( linea, '\0', sizeof(char)*MAX_LINEA );
					}else
					{
						int restante = MAX_LINEA - lastIndex;

						if(dam.transfer_size > restante)
						{
							myMemCpy(&(linea[lastIndex]), buffer_chars, restante);
							log_info(dam_log,"LINEA COMPLETA: %s",linea);

							lastIndex=0;
							memset( linea, '\0', sizeof(char)*MAX_LINEA );
							myMemCpy(&(linea[lastIndex]), buffer_chars+restante, dam.transfer_size-restante);
							log_info(dam_log,"Nueva Linea: %s",linea);
							lastIndex = dam.transfer_size-restante;

						}
						else
						{
							myMemCpy(&(linea[lastIndex]), buffer_chars, dam.transfer_size);
							log_info(dam_log,"Linea Normal: %s",linea);
							lastIndex += dam.transfer_size;

							if(lastIndex == MAX_LINEA)
							{
								log_info(dam_log,"LINEA COMPLETA");
								lastIndex=0;
								memset( linea, '\0', sizeof(char)*MAX_LINEA );
							}
						}
					}

				}
			}
			break;

			//solicitud a El Diego indicando que se requiere hacer un Flush del archivo, necesito parametros
			case FLUSH:{
			}
			break;
			//Tengo que enviar mensaje de crear archivo a mdj con un determinado path y cantidad de lineas necesarias
			case CREAR:{

			void* buffer = malloc(paquete->tamanio_mensaje);
			recv(cliente_socket,&buffer,paquete->tamanio_mensaje,MSG_WAITALL);

			operacion_crear* operacion_crear = deserializar_operacion_crear(buffer);

			log_info(dam_log,"/Procesando/ Crear Archivo, PID: %s, Archivo: %s, Cant_Lineas: %s",operacion_crear->pid,operacion_crear->ruta_archivo,operacion_crear->cant_lineas);

			int* tam_buffer_mdj = malloc(sizeof(int));
			operacion_crear_mdj* operacion_crear_mdj = malloc(sizeof(operacion_crear_mdj));

			operacion_crear_mdj->cant_lineas = operacion_crear->cant_lineas;
			operacion_crear_mdj->ruta_archivo = string_duplicate(operacion_crear->ruta_archivo);
			void* buffer_mdj = serializar_operacion_crear_mdj(operacion_crear_mdj,tam_buffer_mdj);

			log_info(dam_log,"Enviando Peticion MDJ Crear Archivo");

			header_paquete* paquete=malloc(sizeof(header_paquete));
			paquete->tipo_operacion = CREAR_ARCHIVO;
			paquete->tamanio_mensaje = *tam_buffer_mdj;

			send(socket_mdj,paquete,sizeof(header_paquete),0);
			send(socket_mdj,buffer_mdj,(size_t)tam_buffer_mdj,0);

			//Debo hacer un recv del resultado de la operacion de MDJ e informar a SAFA
			free(tam_buffer_mdj);
			free(buffer);
			free(buffer_mdj);
			free(operacion_crear->ruta_archivo);
			free(operacion_crear);
			free(operacion_crear_mdj->ruta_archivo);
			free(operacion_crear_mdj);

			}
			break;
			//debo enviar a mdj borrar determinado archivo
			case BORRAR:{

				void* buffer = malloc(paquete->tamanio_mensaje);
				recv(cliente_socket,&buffer,paquete->tamanio_mensaje,MSG_WAITALL);
				operacion_archivo* operacion_archivo = deserializar_operacion_archivo(buffer);

				log_info(dam_log,"/Procesando/ Borrar Archivo, PID: %s, Archivo: %s,",operacion_archivo->pid,operacion_archivo->ruta_archivo);

				int* tam_buffer_mdj = malloc(sizeof(int));
				operacion_archivo_mdj* operacion_archivo_mdj = malloc(sizeof(operacion_archivo_mdj));
				operacion_archivo_mdj->ruta_archivo = string_duplicate(operacion_archivo->ruta_archivo);

				void* buffer_mdj = serializar_operacion_archivo_mdj(operacion_archivo_mdj,tam_buffer_mdj);

				log_info(dam_log,"Enviando Peticion MDJ Borrar Archivo");

				header_paquete* paquete=malloc(sizeof(header_paquete));
				paquete->tipo_operacion = BORRAR_ARCHIVO;
				paquete->tamanio_mensaje = *tam_buffer_mdj;

				send(socket_mdj,paquete,sizeof(header_paquete),0);
				send(socket_mdj,buffer_mdj,(size_t)tam_buffer_mdj,0);

				//Debo hacer un recv del resultado de la operacion de MDJ e informar a SAFA

				free(tam_buffer_mdj);
				free(buffer);
				free(buffer_mdj);
				free(operacion_archivo->ruta_archivo);
				free(operacion_archivo);
				free(operacion_archivo_mdj->ruta_archivo);
				free(operacion_archivo_mdj);

			}
			break;

			}

		}


	free(paquete);


}
