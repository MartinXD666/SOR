# Sistemas Operativos y Redes

## Trabajo Práctico de hilos y semáforos

1) Para levantar la receta desde un archivo de texto primero declare un apuntador tipo FILE*, que apuntara la posicion de memoria (flujo) donde se encuentra los datos que se cargaran al momento de utilizar la instruccion 'fopen' para abrir el .txt que contiene la receta, con la opcion tipo 'r' para solo lectura.
Una vez abierto el archivo en modo lectura, comienzo a trabajar el texto que contiene. Antes que eso aclaro algunas variables:

-int indice_pasos_param = 0; -->variable que trabajara como indice del array pasos_param[8];
-
-int indice_ingredientes = 0; -->variable que trabajara como indice del array ingredientes[4];
-
-int i = 0; -->variable que controla el indice de la cadena
-
-int c = 0; -->variable que controla que solo ahi una accion por linea
-

El while principal cuya condicion es: (feof(flujo) == 0). Esto significa que mientras la funcion 'feof' que recibe como argumento a el archivo (flujo) devuelva 0 no terminara su ciclo. Dentro de esto la variable 'caracter' de tipo char levantara al texto caracter por caracter, con la funcion 'fgetc' quien recibe como argumento el archivo. En la condicion principal (if(!feof(flujo))) reviso que no sea el fin del archivo(esto lo hice para solucionar un error que tiene la funcion 'feof', el cual la ultima linea del archivo la tomaba 2 veces). Lo siguiente a esto es acorde a como rellene y organize el archivo.txt. La idea general pensada es que tome linea por linea al archivo.txt, para asi poder trabajarlo con una linea y luego con la otra sucesivamente. Para esto, primero tuve que hacer una condicion que reconozca el salto de linea ('\n'), cuya condicion seria que si caracter es distinto de '\n', entonces que me guarde en un array de tipo char(char cadena[]) esos caracteres que se van levantando del archivo.txt. En el caso de que encuentre el '\n', tendra que empezar a separar la cadena que guardo anteriormente. Para esto utilice la funcion 'strtok'(equivalente al split) quien toma dos argumentos: la cadena a separar y el caracter donde separar. En mi caso, en la primera palabra de cada linea del archivo.txt tengo la accion y en las que siguen los ingredientes, entonces puse una condicion que si el valor de la variable tipo int 'c' es menor a 1 setear la accion y que aumente 1 el valor de c. Esto es para que el la proxima palabra esa condicion ya no se cumpla y se valla al else, quien se encargara de setear los ingredientes(que seran las palabras que queden hasta que la funcion 'strtok' devuelva NULL). Una vez terminada la linea, aumentara el indice de pasos, seteara el indice de ingredientes, j, c y vaciara la cadena con la funcion 'memset'.

2) Al crear los 3 hilos que van a simular (equipo1, equipo2, equipo3) y como funcion a ejecutar los 3 tendra la misma(prepararHamburguesa). Como parametro cada hilo tomara un valor tipo int, quien se encargara de representar a cada hilo.

3) Para hacer que cada accion/paso se escriba en un archivo.txt como salida de texto, cree una funcion 'escribirAccion' que es una copia de la funcion 'imprimirAccion', solo que escribirAccion abre el archivo.txt con la opcion tipo 'a'. Llegue a darme cuenta que usar esta opcion resultaria, ya que te va escribiendo cada accion/paso una abajo de la otra sin subcribirse. Luego tambien utilize una funcion llamada 'fprintf', la cual como primer argumento es el archivo y como segundo lo que queres escribir en el archivo.txt.

4) Al momento de controlar la utilizacion de los recursos compartidos para los equipos utilize los 'mutex', el cual bloqueara la entrada de otro hilo2 en el instante que hilo1 este usando ese recurso. Este sera desbloqueado para hilo2 cuando el hilo1 halla terminado su tarea.

5) Pseudocodigo mostrando como se sincrinizan las tareas: wait(P) - post(V)

*Tengo estas tareas a realizar;

cortar_ingredientesI / ajo cebolla perejil
mezclar / ajo cebolla perejil carne_picada
salar / sal mezcla_preparada
armar_medallones / mezcla_armada
cocinar / medallones_preparados 
hornear / panes
cortar_ingredientesII / lechuga tomate
armar_hamburguesas / lechuga_cortada tomate_cortado medallones_cocinados panes_horneados

*Semaforos a utilizar;          *mutex a utilizar;

sem_t sem_mezclar;               mutex_t salero;
sem_t sem_salar;                 mutex_t plancha;
sem_t sem_armarMed;              mutex_t horno;
sem_t sem_cocMed;
sem_t sem_armarHambur;

.
                                                                                      P(sem_armarHambur);
                                                                                      P(sem_armarHambur);
                                                                                      P(sem_armarHambur);
#  cortar_ingredientesI()       hornear()               cortar_ingredientesII()         armar_hamburguesa()
V(sem_mezclar);                V(sem_armarHambur);     V(sem_armarHambur);

P(sem_mezclar);
#  mezclar()
V(sem_salar);

P(sem_salar);
 mutex_lock(salero);
#  salar()
 mutex_unlock(salero);
V(sem_armarMed);

P(sem_armarMed);
#  armar_medallones()
V(sem_cocMed);

P(sem_cocMed);
 mutex_lock(plancha);
# plancha
 mutex_unlock(plancha);
V(sem_armarHambur);
