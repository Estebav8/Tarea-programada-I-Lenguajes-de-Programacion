

#include <stdio.h>//Biblioteca estandar de C.
#include <stdlib.h>//Permite el uso del exit() en caso de error.
#include <string.h>//Habilita el bzero y bcopy.
#include <sys/types.h>//Biblioteca que incluye distintos tipos de procesos para trabajar con sockets.
#include <sys/socket.h>//Biblioteca usada para la implementacion de sockets.
#include <unistd.h>//Uso de fork.
#include <netinet/in.h>//Libreria que nos permite utilizar los headers para resolución de DNS.
#include <netdb.h>//Permite trabajar con las bases de datos que se encuentran en la red.
#include <signal.h> //Permite finalizar los procesos.

//Se declara la cantidad de conecciones que se pueden tener
const int BACK_LOG = 5;

//se comienza a ejecutar el programa
int main(int argc, char *argv[])
{
	//El fork lo que hace es crear un proceso hijo que posee un identificador
	if(!fork()){
		int LevantaSer;//Variable int que almacena los puertos del servidor
		int LevantaCli;//Variable int que almacena los puestos del cliente
		int flag;
		socklen_t LargoCliente; 
		char mensaje[256];//Maneja los mensajes que son enviados por los usuarios
		struct sockaddr_in ServerDireccion, NomCliente;
		char salida[]="Adios\n";

		//Contiene la direccion del servidor
		LevantaSer = socket(AF_INET, SOCK_STREAM, 0); 
		
		int coneccion;//Variable que almacena la IP capturada de consola
		coneccion = atoi(argv[3]);
		
		//En caso de que se de un error en el servidor ya que este debe ser mayor a 0 para que se de la coneccion
		if (LevantaSer < 0) { 
			fprintf(stderr,"Error de Coneccion");
			exit(1);
		}
		    
		//Caracteristicas de la direccion del servidor
		ServerDireccion.sin_family = AF_INET;
		
		ServerDireccion.sin_addr.s_addr = INADDR_ANY;
		//caracteristicas del puerto de coneccion 
		ServerDireccion.sin_port = htons(coneccion);
		
		//Variable a cargo del listener del servidor 
		int ServidorL=bind(LevantaSer, (struct sockaddr *) &ServerDireccion,sizeof(ServerDireccion));
		
		//validacion que permite llevar a cabo el manejo de un error
		if (ServidorL< 0){
		 fprintf(stderr,"Error de Coneccion");//Salida del programa
		 exit(1);
		}
		
		//Listen del servidor
		listen(LevantaSer,5);

		//Verifica que el largo de la direccion del cliente sea igual a las variables de la estructura
		LargoCliente = sizeof(NomCliente);
		
		//Accept del socket
		LevantaCli = accept(LevantaSer, (struct sockaddr *) &NomCliente, &LargoCliente); 

		//En caso de error a la hora de levantar cliente
		if (LevantaCli < 0){
		 fprintf(stderr,"Error con el cliente");
		 exit(1);	 
		}	
		
		//Flag que nos permite llegar a cabo el manejo de errores en los sockets
		while(1){		
			bzero(mensaje,256);//Llena de 0 un arreglo
			flag = read(LevantaCli,mensaje,255);
			
			//Manejo de errores
			if (flag <= 0){
			 fprintf(stderr,"Error de Coneccion");
			 exit(1);
			}
			
			printf("%s\n\a",mensaje);//Printea el mensaje enviado
			//Manejo del error			
			if(strcmp(mensaje,salida)==0){
				//se obtiene el ID del proceso del fork				
				int SocketPrincipal= getpid();								
				SocketPrincipal--;				
				kill(SocketPrincipal, SIGKILL);
				//Asesina el proceso del socket
				break;
			}
		}
		//Manejo de errores
		if (flag < 0){
		 fprintf(stderr,"Errores al escribir");
		 exit(1);
		}
		close(LevantaSer);//Cierra el servidor
		close(LevantaCli);//Cierra el cliente
	}

	else{
		//Variables para el control de datos
		sleep(3);
		int LevantaSer, flag, pCliente; 
		struct sockaddr_in IDServidor;
		struct hostent *servidor; 
		char mensaje[256]; //Trabaja con los mensajes del programa
		char MensajeRecibido[256];//Se encarga de los mensajes recibidos
		char salida[]="Adios\n";
		//int id=getpid();

		
		pCliente = atoi(argv[2]);
		LevantaSer = socket(AF_INET, SOCK_STREAM, 0); //Comienza el servidor
		
		//Manejo de errores
		if (LevantaSer < 0){
			fprintf(stderr,"Error de Coneccion");
			exit(1);
		}
		
		//Obtiene el nombre del servidor
		servidor = gethostbyname(argv[1]); 
		
		//Manejo de errores
		if (servidor == NULL) {
			fprintf(stderr,"Error de Coneccion");
			exit(1);
		}
		
		//Obtiene la direccion del servidor
		IDServidor.sin_family = AF_INET; 
		bcopy((char *)servidor->h_addr,(char *)&IDServidor.sin_addr.s_addr, servidor->h_length); 
		IDServidor.sin_port = htons(pCliente);
		while(1==1){
			//Control de errores
			flag=connect(LevantaSer,(struct sockaddr *) &IDServidor,sizeof(IDServidor));
			if (flag >= 0){
				break;
			}
		}
		//Control de errores
		if (flag < 0){
			fprintf(stderr,"Error de Coneccion");
			exit(1);
		}	
		//REalizacion del Chat
		while(1){	
			printf("\n");
			printf("Enviado:");
			bzero(mensaje,256); 
			bzero(MensajeRecibido,256);
			fgets(mensaje,255,stdin); 
			sprintf(MensajeRecibido,"\nRecibido : %s",mensaje);
			if(strcmp(mensaje,salida)==0){			
				flag = write(LevantaSer,mensaje,strlen(mensaje)); 
				if (flag <= 0){
					fprintf(stderr,"Error al leer el mensaje");
					exit(1);
				}
				int SocketPrincipal= getpid(); 
				SocketPrincipal++;				
				kill(SocketPrincipal, SIGKILL);
				break;
			}
			else{
				flag = write(LevantaSer,MensajeRecibido,strlen(MensajeRecibido));
				if (flag <= 0){
					fprintf(stderr,"Error en el servidor");
					exit(1);
				}
			}			
		}
		close(LevantaSer);
	}
	return 0;
	  
}
