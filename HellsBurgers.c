
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define LIMITE 50

//declaro e inicializo los mutex
pthread_mutex_t salero = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t plancha = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t horno = PTHREAD_MUTEX_INITIALIZER;

//creo estructura de semaforos 
struct semaforos {
    	sem_t sem_mezclar;
    	sem_t sem_salar;
	sem_t sem_armarMed;
	sem_t sem_cocMed;
	sem_t sem_armarHambur;

};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];
   
};

//creo los parametros de los hilos 
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[8];
};

void* gano(void *data);

//funcion para escribir la salida en archivo de texto
void* escribirAccion(void *data, void *accion) {

	FILE *fp = fopen("salida.txt", "a");

	if(fp == NULL) {
		printf("Error al abrir el archivo!");
	}

	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accion) == 0){
		fprintf(fp,"\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		fprintf(fp,"\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							fprintf(fp,"\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
	fclose(fp);
}

//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {

	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
}

void* cortar_ingredientesI(void *data) {

	char *accion = "cortar_ingredientesI";

	struct parametro *midata = data;

	imprimirAccion(midata,accion);
	escribirAccion(midata,accion);

	usleep(200000);

	sem_post(&midata->semaforos_param.sem_mezclar);

	pthread_exit(NULL);
}

//----------------------------------------------------------

void* mezclar(void *data) {

	struct parametro *midata = data;

	sem_wait(&midata->semaforos_param.sem_mezclar);
	char *accion = "mezclar";

	imprimirAccion(midata,accion);
	escribirAccion(midata,accion);

	usleep(2000000);
	sem_post(&midata->semaforos_param.sem_salar);

	pthread_exit(NULL);
}

//-------------------------------------------------------
void* salar(void *data){

	struct parametro *midata = data;

	sem_wait(&midata->semaforos_param.sem_salar);
	pthread_mutex_lock(&salero);

	char *accion = "salar";

	imprimirAccion(midata,accion);
	escribirAccion(midata,accion);

	usleep(2000000);

	pthread_mutex_unlock(&salero);
	sem_post(&midata->semaforos_param.sem_armarMed);

	pthread_exit(NULL);
}

//----------------------------------------------------------
void* armar_medallones(void *data){

	struct parametro *midata = data;

	sem_wait(&midata->semaforos_param.sem_armarMed);

	char *accion = "armar_medallones";

	imprimirAccion(midata,accion);
	escribirAccion(midata,accion);

	usleep(2000000);
	sem_post(&midata->semaforos_param.sem_cocMed);

	pthread_exit(NULL);
}

//-------------------------------------------------------
void* cocinar(void *data){

	struct parametro *midata = data;

	sem_wait(&midata->semaforos_param.sem_cocMed);
	pthread_mutex_lock(&plancha);

	char *accion = "cocinar";

	imprimirAccion(midata,accion);
	escribirAccion(midata,accion);
	
	usleep(5000000);

	pthread_mutex_unlock(&plancha);
	sem_post(&midata->semaforos_param.sem_armarHambur);	

	pthread_exit(NULL);
}

//------------------------------------------------------------
void* hornear(void *data){

	pthread_mutex_lock(&horno);
	char *accion = "hornear";

	struct parametro *midata = data;

	imprimirAccion(midata,accion);
	escribirAccion(midata,accion);

	usleep(10000000);
	pthread_mutex_unlock(&horno);
	sem_post(&midata->semaforos_param.sem_armarHambur);

	pthread_exit(NULL);
}

//------------------------------------------------------------
void* cortar_ingredientesII(void *data) {

	char *accion = "cortar_ingredientesII";

	struct parametro *midata = data;

	imprimirAccion(midata,accion);
	escribirAccion(midata,accion);

	usleep(2000000);

	sem_post(&midata->semaforos_param.sem_armarHambur);	


	pthread_exit(NULL);
}

//------------------------------------------------------------
void* armar_hamburguesas(void *data){

	FILE *fp = fopen("salida.txt", "a");

	if(fp == NULL) {
		printf("Error al abrir el archivo!");
	}

	struct parametro *midata = data;

	sem_wait(&midata->semaforos_param.sem_armarHambur);
	sem_wait(&midata->semaforos_param.sem_armarHambur);
	sem_wait(&midata->semaforos_param.sem_armarHambur);

	char *accion = "armar_hamburguesas";

	imprimirAccion(midata,accion);
	escribirAccion(midata,accion);

	usleep(2000000);
	
	printf("\tEquipo %d termino de armar las hamburguesas!\n", midata->equipo_param);
	fprintf(fp ,"\tEquipo %d termino de armar las hamburguesas!\n", midata->equipo_param);

	fclose(fp);
	gano(midata);

	pthread_exit(NULL);
}

//------------------------------------------------------------

void* gano(void *data){

	FILE *fp = fopen("salida.txt", "a");

	if(fp == NULL) {
		printf("Error al abrir el archivo!");
	}

	struct parametro *midata = data;

	printf("\tEquipo %d gano! \n", midata->equipo_param);
	fprintf(fp ,"\tEquipo %d gano! \n", midata->equipo_param);

	fclose(fp);
	exit(-1);

}

//------------------------------------------------------------

void* prepararHamburguesas(void *i){

	//variables semaforos
	sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_armarMed;
	sem_t sem_cocMed;
	sem_t sem_armarHambur;

	//variables hilos
	pthread_t p1;
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	pthread_t p8;

	//numero del equipo
	int e = *((int *)i);

	printf("Ejecutando equipo %d \n", e);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//-----seteo los valores al struct-----

	//seteo numero de grupo
	pthread_data->equipo_param = e;

	//seteo semaforos
	
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_salar = sem_salar;
	pthread_data->semaforos_param.sem_armarMed = sem_armarMed;
	pthread_data->semaforos_param.sem_cocMed = sem_cocMed;
	pthread_data->semaforos_param.sem_armarHambur = sem_armarHambur;


	FILE * flujo = fopen("receta.txt", "r");

	if(flujo == NULL) {
		printf("Error al abrir el archivo!");
	}

	int indice_pasos_param = 0;
	int indice_ingredientes = 0;
	char caracter;
	char salto = '\n';
	int j=0;
	int c=0;
	char cadena1[80];

	while(feof(flujo) == 0) {
		caracter = fgetc(flujo); //leer caracter por caracter
		if(!feof(flujo)) {	// --> para imprimir solo una vez la ultima linea
			if(caracter != salto) {
				cadena1[j] = caracter;
				j++;

			}else{
				//seteo las acciones y los ingredientes
				char delimitador[] = " ";
    				char *token = strtok(cadena1, delimitador);
    				if(token != NULL){
        				while(token != NULL){
            						
						if(c < 1) {
							strcpy(pthread_data->pasos_param[indice_pasos_param].accion, token);
							c++;
							
						}else{
				                	token = strtok(NULL, delimitador);
							if(token != NULL){
								strcpy(pthread_data->pasos_param[indice_pasos_param].ingredientes[indice_ingredientes], token);
								indice_ingredientes++;
							}
						}
        				}
					indice_pasos_param++;
					indice_ingredientes = 0;
					j = 0;
					c = 0;
					memset(cadena1, 0, 80);
    				}
			}

		}
	}

	//inicializo los semaforos
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarMed),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cocMed),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarHambur),0,0);

	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    	int rc;
    	rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar_ingredientesI,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	
	rc = pthread_create(&p2, NULL, mezclar, pthread_data);
	rc = pthread_create(&p3, NULL, salar, pthread_data);
	rc = pthread_create(&p4, NULL, armar_medallones, pthread_data);
	rc = pthread_create(&p5, NULL, cocinar, pthread_data);
	rc = pthread_create(&p6, NULL, hornear, pthread_data);
	rc = pthread_create(&p7, NULL, cortar_ingredientesII, pthread_data);
	rc = pthread_create(&p8, NULL, armar_hamburguesas, pthread_data);
	
	//join de todos los hilos
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
	pthread_join (p4,NULL);
	pthread_join (p5,NULL);
	pthread_join (p6,NULL);
	pthread_join (p7,NULL);
	pthread_join (p8,NULL);


	//valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

	 
	//destruccion de los semaforos 
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	sem_destroy(&sem_armarMed);
	sem_destroy(&sem_cocMed);
	sem_destroy(&sem_armarHambur);

	//destruccion de los mutex
	pthread_mutex_destroy(&salero);
	pthread_mutex_destroy(&plancha);
	pthread_mutex_destroy(&horno);

	
	fclose(flujo);

	pthread_exit(NULL);
	return NULL;
}


int main(){

		//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;

	pthread_t equipo1, equipo2, equipo3;

	rc = pthread_create(&equipo1, NULL, prepararHamburguesas, equipoNombre1);
	rc = pthread_create(&equipo2, NULL, prepararHamburguesas, equipoNombre2);
	rc = pthread_create(&equipo3, NULL, prepararHamburguesas, equipoNombre3);


	pthread_join(equipo1, NULL);
	pthread_join(equipo2, NULL);
	pthread_join(equipo3, NULL);

	if (rc){
       		printf("Error:unable to create thread, %d \n", rc);
       		exit(-1);
     	} 

	pthread_exit(NULL);
	return 0;
}


//Para compilar:   gcc HellsBurgers.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
