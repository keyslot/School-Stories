/* 
 * Seminario de Sistemas Inteligentes.
 * Ejercicio de aprendizaje - "Mono que aprende"
 * Ortega Vallejo Raúl Antonio
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BOTONES 3                // BOTONES PARA EL MONO
#define LIMITE (BOTONES-1)       // ( BOTONES - PREMIO )
#define PORCENTAJE 100           // PORCENTAJE MAXIMO, PROBABILISTICO
#define DELAY 500                // Deleay Barra de aprendizaje (sleep)
#define TIEMPO 60000             // Semilla del random (sleep)
#define LOG_FILE "bitacora.log"  // Ruta del archivo log
/*  
 * Propio Booleano *
 */
#define bool int 
enum {False,True}; 

/*
 * Estructura del Botón
 */
typedef struct {
	int numero;     // Número del Boton
	int presionado; // Se ha presionado el boton // (MEMORIA - EXPERIENCIA)
	float valor;    // Valor de la Probabilistico
}TIPO_B;
/*
 * Referencias
 */
TIPO_B BOTON [BOTONES];     // Botones
TIPO_B *PLATANO = NULL;     // Premio : Platano
TIPO_B *PRESIONADO = NULL;  // Botón actual presionado


char *accion = NULL;        //  Una accion
FILE *bitacora = NULL;

float Y = 2; // Constante (DEFAULT)
int ciclos = 100; // Ciclos del Programa (DEFAULT)

void
muestraBitacora(void)
{
	if(!bitacora) return;
	rewind(bitacora);
	char aux = '\0';
	while((aux = fgetc(bitacora)) != EOF)
		putchar(aux);
	fflush(stdout);
}


/* Generar numeros Aleatorios */
unsigned int
numeroAleatorio(unsigned int max) 
{
	time_t t;
	usleep(TIEMPO);
	srand ((unsigned int) time(&t));
	return rand() % (max + 1);
}

void 
asignaProbabilidad(void) 
{ 	
       int i; 
       float n;          // un numero n
       float ultimo;     // ultimo valor
       float comprueba;  // comprobacion de sumatoria = 100
       accion = "ASIGNANDO PROBABILIDAD";
       while (1) {
	       comprueba = 0;
	       n = ultimo = PORCENTAJE;
	       for (i = 0; i < LIMITE; i++) {
		       n = numeroAleatorio (n);
		       BOTON[i].valor = n;
		       BOTON[i].numero = i + 1; // Numero del Boton
		       BOTON[i].presionado = False;
		       ultimo -= n;
		       comprueba += n;
		       // Rechazar valores negativos
		       if ((n = PORCENTAJE - n) < 0) 
			       break;
	       }
	       comprueba += ultimo;
	       if (ultimo >= 0 && comprueba == PORCENTAJE) {
		       BOTON[i].valor = ultimo;
		       BOTON[i].numero = i + 1; // Numero del Boton
		       break;
	       }
       }
}

TIPO_B *
regresaMayor(void) 
{
	int i;
	TIPO_B *mayor = &BOTON[0];
	for (i = 0; i < BOTONES; i++) {
		if (mayor->valor < BOTON[i].valor) {
			mayor = &BOTON[i];
		}
	}
	//printf("El mayor es %f\n",mayor->valor);
	return mayor;
}

bool 
validaSumatoria(void)
{
	int i;
	float sumatoria;
	for (sumatoria = 0, i = 0;  i < BOTONES; i++) {
		sumatoria += BOTON[i].valor;
	  }
	if (sumatoria == PORCENTAJE) {
	       	return True;
	}
	/*
	 * EXPERIMENTAL: si existen más de 2 botones para el mono
	 */
	else if (BOTONES > 2 && sumatoria >= PORCENTAJE) {
		sumatoria -=  PORCENTAJE;
		regresaMayor()->valor -= sumatoria;
		return True;
	}
	else {  
		/* 
		 * Imprimir sumatoria insatifactoria:
		*/
		printf("\rSumatoria insatisfactoria = %f",sumatoria); 
	       	return False;
	}
}

void 
presionaBoton(void) 
{
	int elegido;
	/********************** METODO 1**********************************/
	// Considerando las Expericencias, y el total de limite de botones
	while (1) {
		elegido =  numeroAleatorio(LIMITE); 
		 // DESICIÓN 1:El mono elige boton no nulo 
	       	if (BOTON[elegido].valor > 0) {
			break;
		    }
		 // DESICIÓN 1:El mono elige botones nulos, y no se han presionado
		else if (BOTON[elegido].presionado == False) {
		     break;
		}
	   }
	BOTON[elegido].presionado = True;
	PRESIONADO = &BOTON[elegido];
	/********************** METODO 2********************************\
	// Considerando una mayor aleatoriedad, basado en el porcentaje
	int i;
	elegido = numeroAleatorio (PORCENTAJE);
	for (i = 0; i < BOTONES;  i++ ) {
		if (elegido <= BOTON[i].valor) {
	           if ( BOTON[i].valor == 0 && 
				BOTON[i].presionado == False) {
		        BOTON[i].presionado = True;
			break;
			 }
		    else if (BOTON[i].valor > 0 ) 
			     break;
		}
		if (i + 1 == BOTONES) {
			i = 0;
			elegido = numeroAleatorio (PORCENTAJE);
		}


	    }
	PRESIONADO = &BOTON[i];
	\**************************************************************/
}

void 
castigo(void) 
{
	int i;
	float aux;
	accion = "CASTIGANDO";
	aux = PRESIONADO->valor - Y;
	// Impide decrementar a 0 el botón presionado
	if (aux >= 0) {
	   PRESIONADO->valor = aux;
	  }
	else {
		regresaMayor()->valor += aux;
		PRESIONADO->valor = 0;
	  }
	for (i = 0; i < BOTONES; i++) {
		if (&BOTON[i] != PRESIONADO) 
			BOTON[i].valor += (Y / LIMITE);
	   }
}

void
premio(void)
{
	int i;
	float  aux;
	accion = "PREMIANDO";
	PRESIONADO->valor += Y;
	/* 
	 * Decrementando los demás valores
	 */
	for (i = 0; i < BOTONES; i++) { 
		if (&BOTON[i] != PRESIONADO) {
		     aux = BOTON[i].valor - (Y / LIMITE); // Distribuye
		     if (aux >= 0)  {
			     BOTON[i].valor = aux;
		     }
		     else {
			     BOTON[i].valor = 0;
			     PRESIONADO->valor += aux;
		     }
		}
	}
}

void
programa(void)
{
	presionaBoton();
	if (PRESIONADO == PLATANO)  
		premio();
	else 
		castigo ();
}

void
interfaz(void) 
{
    int i;
    float aux;
    char espacio = '\0';
    fprintf(bitacora,"\n|");
    for (i=0; i < BOTONES; i++) {
	    aux = BOTON[i].valor;
	    /* Validaciones para darle estilo a la impresión */
	    if ((aux >= 0 && aux <= 9.500000) || 
		             aux <= -99.500000 || aux <= -9.500000) { 
		    espacio = ' ';
	        }
	    else  {
		    espacio = '\0';
	     }
	    fprintf(bitacora," BOTON_%d = %f %c",i + 1, aux,espacio);
            fprintf(bitacora,"|");
    }
    // Se ha presionado algún boton
    if (PRESIONADO)
	    fprintf(bitacora," ELIGIENDO BOTON_%d|",PRESIONADO->numero);
          
    fprintf(bitacora," Accion: %s", accion);

}

void
imprimeMono(void)
{
	 puts("\nc(..)o ");
	 puts("  (o)");
}

void muestraBarraProceso(void)
{
	printf ("\r"); 
	printf (":Aprendizaje: ");
	int i;
	for (i=0; i <= ciclos / 5 ; i++) {
	        fflush(stdout);
		printf(" □");
		usleep(DELAY);
	}
}

bool
continua(void)
{
	if ( ciclos > 0  && validaSumatoria() ) {
		ciclos--;
		return True;
	}
	else {
		return False;
	}
}

void
limpiarBitacora(void) 
{
	if (bitacora) {
		fclose(bitacora);
		bitacora = NULL;
	}
}

void
crearBitacora(void)
{
	limpiarBitacora();
	if (!(bitacora = fopen(LOG_FILE,"w+"))){
		perror("Imposible crear bitacora");
	        exit(EXIT_FAILURE);
	}
}

void
configuracionUsuario(void)
{
	//------ USUARIO -------------//
	int i;
	printf("\nAsignar platano en BOTON_");
	scanf("%d",&i);
	PLATANO = &BOTON[i-1];	
	printf("Asignar constante Y:");
	scanf ("%f",&Y);
	printf("Asignar ciclos:");
	scanf ("%d",&ciclos);
	//----------------------//
}

void
autoConfiguracion(void) 
{
	int platano = numeroAleatorio(LIMITE);
	PLATANO = &BOTON[platano];
	ciclos = 60;
	printf("\nEl platano esta en el BOTON_%d",platano+1);
	printf("\nCiclos: %d\n",ciclos);
	Y = 10;
}

void
configura(void) 
{
        char respuesta = '\0';
	while (1) {
		asignaProbabilidad(); 
		crearBitacora();
		interfaz();
		muestraBitacora(); 
		printf("\n¿ACEPTAR? s/n _");
		scanf("\n%c",&respuesta);
	        if (respuesta != 'n') 
			break;
	     }
	configuracionUsuario (); 
	//autoConfiguracion();
	imprimeMono(); // Imprime cara del mono
	
}

void
salida(void)
{
	puts("\n\nBITACORA");
	muestraBitacora();
	// Limpieza...
	limpiarBitacora();
	accion = NULL;
	PRESIONADO = PLATANO = NULL;
}

int
main(void) 
{
	configura();
	while (continua()) {
		programa(); // Toma de desiciones
		interfaz(); // Generando bitacora
		muestraBarraProceso(); // Aprendizaje
	   }
	salida (); 
	return 0;
}
