/*---------------------------*/
/*   CONSTANTES  GENERALES   */
/*---------------------------*/
#define NULO 0
#define MAXLIN 1000
#define BLANCO (char)' '
#define NLINEA (char)'\n'
#define TABULA (char)'\t'

/*-----------------------------*/
/*   CONSTANTES  ESPECIFICAS   */
/*-----------------------------*/
#define NOMBRE "Name"
#define NUMEST "NumStates"
#define NUMEDG "NumEdges"
#define TOTCHK "TotalChecksum"
#define ESTADO "State"
#define CHKSUM "checksum"
#define INITIA "initial"
#define FINALE "final"
#define OUTPUT "output"
#define PROBAB "probability"
#define CHKSUMA "c"
#define INITIAA "i"
#define FINALEA "f"
#define OUTPUTA "o"
#define PROBABA "p"

#define LAMBDA ""

#define SI_NOMBRE 1
#define SI_NUMEST 1
#define SI_NUMEDG 1
#define SI_TOTCHK 1
#define SI_CHK_EST 1
#define SI_OUT_E_A 2
#define SI_INI_EST 4
#define SI_FIN_EST 8
#define NO_OPC_EST 16
#define SI_PRO_ARI 1
#define NO_OPC_ARI 4

#define CON_OUTPUT 1
#define SIN_OUTPUT 2
#define AUTOMATA    4
#define AUTO_PROB   1
#define TRANSDUCTOR 2
#define TRAN_PROB   3

#define LANDSCAPE 1
#define PORTRAIT 2

/*---------------*/
/*   FUNCIONES   */
/*---------------*/

extern char *GetError(int error);

/*----------------------------------------------------------------------------*/

extern char *ObtenLinea(FILE *fd, char *linea, int max);

/*----------------------------------------------------------------------------*/

extern int LineaEnBlanco(char *linea);

/*----------------------------------------------------------------------------*/

extern int LeerNombre(char *linea, char *auxlinea);

/*----------------------------------------------------------------------------*/

extern int LeerNumEstados(char *linea, int *ptr_est);

/*----------------------------------------------------------------------------*/

extern int LeerNumAristas(char *linea, int *ptr_ari);

/*----------------------------------------------------------------------------*/

extern int LeerTotalChecksum(char *linea, double *ptr_tchk);

/*----------------------------------------------------------------------------*/

extern int LeerEstado(char *linea, char *ident, double *checksum,
                      double *initial, double *final, char *output);

/*----------------------------------------------------------------------------*/

extern int LeerArista(char *linea, char *origen, char *destino, char *simbolo,
	              double *prob, char *output);

/*----------------------------------------------------------------------------*/

extern void EscribeNombre(char *linea, char *auxlinea);

/*----------------------------------------------------------------------------*/

extern void EscribeNumEstados(char *linea, int est);

/*----------------------------------------------------------------------------*/

extern void EscribeNumAristas(char *linea, int ari);

/*----------------------------------------------------------------------------*/

extern void EscribeTotalChecksum(char *linea, double tchk, int tipored);

/*----------------------------------------------------------------------------*/

extern void EscribeEstado(char *linea, int ident, double checksum,
          double initial, double final, char *output, int tipored);

/*----------------------------------------------------------------------------*/

extern void EscribeArista(char *linea, int origen, int destino,
                         char *simbolo, double prob, char *output, int tipored);

/*----------------------------------------------------------------------------*/

extern void Lineas80Caracteres(void);

/*----------------------------------------------------------------------------*/
