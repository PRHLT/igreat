/*---------------------------------------------------------------*/
/*  Tipos para representar una Red de Estados Finitos Generica   */
/*----------------------------------------------------------------------------*/
/*
    La red se representara con un registro (T_REDGEN) que contendra
    algunos datos de la red (nombre, num estados, num aristas, num
    simbolos de salida, num estados iniciales, num estados finales,
    etc.). Y principalmente, tendra dos vectores que contendran los
    estados y las aristas. A su vez, cada estado y cada arista (en
    los vectores) sera un registro que contendra informacion sobre
    el estado y la arista, respectivamente.
    
    Cada estado contiene cuatro datos: estado inicial, estado final,
    checksum y cadena de salida. Y ademas, tiene la lista de aristas
    que salen del estado. En realidad, solo tiene el puntero a la
    primera arista que es un indice en el vector de aristas. Las
    siguientes aristas salientes de ese estado se obtienen a partir
    de la primera con el campo 'ssig' del registro de cada arista.
    Este campo 'ssig' vuelve a ser un indice en el vector de aristas.
    
    Las cadenas de salida de se representan en el estado como un
    puntero (indice) y un offset (longitud) en un vector de cadenas
    de salida, 'out'. En este vector se insertan secuencialmente
    todas las cadenas de salida (de aristas o de estados) de la red.
    Y 'pt' sera el indice en 'out' del primer simbolo de la cadena,
    y 'ot' es la longitud de la cadena, de forma que el ultimo simbolo
    de la cadena es 'pt+ot-1' en 'out'.
    
    El vector de aristas viene a ser un bloque de memoria dividido
    en registros, cada cual representa a una arista. Los datos de
    una arista son: el simbolo (de entrada), la cadena de salida,
    la probabilidad y el estado destino. El estado destino es un
    indice en el vector de estados. La cadena de salida se repre-
    senta igual que en los estados. El simbolo es un numero que
    se correpondera con una entrada en un diccionario. Los
    simbolos de salida tambien son numeros que se corresponden
    con entradas en un diccionario. Ademas, en la arista, el
    campo 'ssal' lleva a la siguiente arista del estado corres-
    pondiente o vale INDNULO.
*/

#define MAXLINLR     256
#define Ending       -3
#define Unknown      -2
#define Beginning    -1
#define Transit       0
#define Backoff       1
#define IN	      1
#define OUT	      2
#define Training      3
#define Decoding      4

#include "giati.h"
#include "hash.h"

typedef struct lint {
	int n;
	char *word;
	struct lint *next;
} l_int;

typedef struct t_arista
{
  //int        pt;    /* Indice del primer simbolo de salida de la arista */
  //int        ssal;  /* Indice de la siguiente arista en la lista */
  //char       ot;    /* Numero de simbolos de la cadena de salida de la arista */
  //int        simb;  /* Simbolo terminal de entrada */

  int        origen;  /* Estado de origen de la arista */
  int        input;
  l_int      *output;
  double     prob[MaxProb];  /* Frecuencia o probabilidad de la arista */
  int	     np;
  int        dest;  /* Estado de destino de la arista */
  struct t_arista *siguiente;  /* Indice de la siguiente arista en la lista */
} T_ARISTA;

typedef struct
{
  //int     pt;       /* Indice del primer simbolo de salida del estado */
  //char    ot;       /* Numero de simbolos de la cadena de salida del estado */
  //int     psal;     /* Indice a las aristas que salen de este estado */
  //double  checksum; /* Frecuencia (o probabilidad = 1) total del estado */
  //char    *output;

  int	   active;
  double   initial;  /* Frecuencia o probabilidad de ser estado Inicial */
  double   final;    /* Frecuencia o probabilidad de ser estado Final */
  T_ARISTA *cabeza;  /* Las aristas del estado */
  T_ARISTA *cola;
  T_ARISTA *BACKOFF;  
  T_ARISTA *UNKNOWN;  
} T_ESTADO;

typedef struct
{
  //int        *fin;                      /* Estados Finales */
  //DICCIONARIO *alfaent, *alfasal, *alfaest;
  //int        maxE,maxA,maxO,maxI,maxF;  /* maximos de lo anterior disponibles */
  //int        *out;                      /* Salidas (en traductores) */
  //T_ARISTA   *arc;                      /* Aristas */
  //int        tipo;                      /* Tipo de red (aut., trad., etc.) */
  //int        no,nf;            /* numeros de est, arc, out, ini, fin */
  //double     totchk;                    /* Frecuencia ("checksum") Total */

  h_t	     *table, *table_in, *table_out;
  int	     n_in, n_out;
  char 	     **alfa_in, **alfa_out;   
  char       nombre[MAXLINLR];          /* Como su nombre indica */
  int        ne;	            	/* numero de est, arc, out, ini, fin */
  T_ESTADO   *est;                      /* Estados */
  int        ini;                      	/* Estado Inicial */
} T_REDGEN;

extern T_REDGEN *CrearRed(int ne, int hts);
extern l_int *insert(l_int *output, int n, char *simb);
extern l_int *append(l_int *a, l_int *b, int *UNK, char *simb, int flag);
extern void enter_state(T_REDGEN *A, int state, double ini, double fin);
extern void enter_transition(T_REDGEN *A, int stage, int type, int source,
			     int target, char *symbols, double weight, lexicon *l);
extern void get_final_probs(T_REDGEN *A);
extern T_REDGEN *read_fsm(char *file, int hts);
extern void write_fsm(T_REDGEN  *A, char *file, int n, struct options *opt);
extern void LiberarRed(T_REDGEN *A);
extern int LeerRed(FILE *fd, T_REDGEN *A, int numred, char *nombre);
extern void EscribirRed(T_REDGEN *A, int renom, int desplz);
