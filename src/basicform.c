/*--------------*/
/*   IMPORTAR   */
/*--------------*/
#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "basicform.h"
/*
#include "basicos.h"
*/
#define FALSO (1==2)
#define VERDAD (1==1)
#define ASIGMEM(tipo,tam) (tipo*)malloc((tam)*sizeof(tipo))
/*----------------------------------------------------------------------------*/

/*---------------*/
/*   FUNCIONES   */
/*----------------------------------------------------------------------------*/
int   linea80=FALSO;
/*----------------------------------------------------------------------------*/

/*---------------*/
/*   FUNCIONES   */
/*---------------*/

/*----------------------------------------------------------------------------*/
int Reservada(char *linea)
{
  int res;

  res = ( (strcmp(linea,NOMBRE) == 0) || (strcmp(linea,NUMEST) == 0)  ||
          (strcmp(linea,NUMEDG) == 0) || (strcmp(linea,TOTCHK) == 0)  ||
          (strcmp(linea,ESTADO) == 0) || (strcmp(linea,"=") == 0)     ||
          (strcmp(linea,CHKSUM) == 0) || (strcmp(linea,CHKSUMA) == 0) ||
          (strcmp(linea,INITIA) == 0) || (strcmp(linea,INITIAA) == 0) ||
          (strcmp(linea,FINALE) == 0) || (strcmp(linea,FINALEA) == 0) ||
          (strcmp(linea,OUTPUT) == 0) || (strcmp(linea,OUTPUTA) == 0) ||
          (strcmp(linea,PROBAB) == 0) || (strcmp(linea,PROBABA) == 0) );

  return(res);
}
/*----------------------------------------------------------------------------*/
char *GetError(int error)
{
  char *auxlinea;

  auxlinea=ASIGMEM(char,MAXLIN);

  switch (error)
  {
    case  -1:  strcpy(auxlinea,"Definicion de nombre erronea. No existe nombre asociado."); break;
    case  -2:  strcpy(auxlinea,"Definicion de nombre erronea. Existen demasiados nombres."); break;
    case  -3:  strcpy(auxlinea,"Definicion de numero de estados erronea. No existe numero asociado."); break;
    case  -4:  strcpy(auxlinea,"Definicion de numero de estados erronea. Existen demasiados simbolos."); break;
    case  -5:  strcpy(auxlinea,"Numero de estados no valido."); break;
    case  -6:  strcpy(auxlinea,"Definicion de numero de aristas erronea. No existe numero asociado."); break;
    case  -7:  strcpy(auxlinea,"Definicion de numero de aristas erronea. Existen demasiados simbolos."); break;
    case  -8:  strcpy(auxlinea,"Numero de aristas no valido."); break;
    case  -9:  strcpy(auxlinea,"Definicion de checksum total erroneo. No existe numero asociado."); break;
    case -10:  strcpy(auxlinea,"Definicion de checksum total erroneo. Existen demasiados simbolos."); break;
    case -11:  strcpy(auxlinea,"Numero de checksum total no valido."); break;
    case -12:  strcpy(auxlinea,"Definicion de opcion erronea. Esperaba una palabra reservada."); break;
    case -13:  strcpy(auxlinea,"Definicion de checksum erronea. Esperaba el simbolo '='."); break;
    case -14:  strcpy(auxlinea,"Numero no valido para checksum."); break;
    case -15:  strcpy(auxlinea,"Definicion de inicial erronea. Esperaba el simbolo '='."); break;
    case -16:  strcpy(auxlinea,"Definicion de final erronea. Esperaba el simbolo '='."); break;
    case -17:  strcpy(auxlinea,"Definicion de output erronea. Esperaba el simbolo '='."); break;
    case -18:  strcpy(auxlinea,"El output del estado o de la arista no puede ser una palabra reservada."); break;
    case -19:  strcpy(auxlinea,"Definicion de estado erronea. No existe etiqueta asociada."); break;
    case -20:  strcpy(auxlinea,"El identificador del estado no puede ser una palabra reservada."); break;
    case -21:  strcpy(auxlinea,"Definicion multiple de checksum. Me quedo con la ultima."); break;
    case -22:  strcpy(auxlinea,"Definicion multiple de initial. Me quedo con la ultima."); break;
    case -23:  strcpy(auxlinea,"Definicion multiple de final. Me quedo con la ultima."); break;
    case -24:  strcpy(auxlinea,"Definicion multiple de output. Me quedo con la ultima."); break;
    case -25:  strcpy(auxlinea,"Definicion de probabilidad erronea. Esperaba el simbolo '='."); break;
    case -26:  strcpy(auxlinea,"Numero no valido para la probabilidad de una arista."); break;
    case -27:  strcpy(auxlinea,"Definicion de tipo erronea. Esperaba el simbolo '='."); break;
    case -28:  strcpy(auxlinea,"El tipo de la arista no puede ser una palabra reservada."); break;
    case -29:  strcpy(auxlinea,"Definicion de arista erronea. El estado origen no puede ser una palabra reservada."); break;
    case -30:  strcpy(auxlinea,"Definicion de arista erronea. No aparecen todos los argumentos obligatorios."); break;
    case -31:  strcpy(auxlinea,"Definicion de arista erronea. El estado destino no puede ser una palabra reservada."); break;
    case -32:  strcpy(auxlinea,"Definicion de arista erronea. El simbolo de la arista no puede ser una palabra reservada."); break;
    case -33:  strcpy(auxlinea,"Definicion multiple de probabilidad. Me quedo con la ultima."); break;
    case -34:  strcpy(auxlinea,"Definicion multiple de tipo. Me quedo con la ultima."); break;
    case -35:  strcpy(auxlinea,"String no cerrada por las correspondientes comillas."); break;
    case -36:  strcpy(auxlinea,"En el fichero hay lineas imposibles de interpretar."); break;
    case -37:  strcpy(auxlinea,"Definicion de numero de estados duplicada."); break;
    case -38:  strcpy(auxlinea,"Definicion de numero de aristas duplicada."); break;
    case -39:  strcpy(auxlinea,"Definicion de checksum total duplicada."); break;
    case -40:  strcpy(auxlinea,"Esta arista no concuerda con las anteriores en el tipo de red que describen."); break;
    case -41:  strcpy(auxlinea,"Este estado no concuerda con los anteriores en el tipo de red que describen."); break;
    case -42:  strcpy(auxlinea,"El numero de estados realmente leidos no coincide con NumStates."); break;
    case -43:  strcpy(auxlinea,"El numero de aristas realmente leidas no coincide con NumEdges."); break;
    case -44:  strcpy(auxlinea,"La suma de los valores (frec. o prob.) de los estados iniciales no coincide con TotalChecksum."); break;
    case -45:  strcpy(auxlinea,"La suma de los valores (frec. o prob.) de las aristas mas el de final no coincide con el checksum del estado."); break;
    case -46:  strcpy(auxlinea,"Numero no valido para la probabilidad de estado inicial."); break;
    case -47:  strcpy(auxlinea,"Numero no valido para la probabilidad de estado final."); break;
  }
  return(auxlinea);
}
/* ## error ==  0  No se ha leido lo que se esperaba leer pero no quiere decir que haya error. ## */
/* ## error  >  0  Se ha leido exactamente lo que se esperaba leer. ## */
/*----------------------------------------------------------------------------*/
void EliminaComentario(char *linea)
{
  char *ptr;
  int  fin;

  fin=FALSO;
  ptr=linea;
  while (!fin)
  {
    while ((*ptr != '\\') && (*ptr != '"') && (*ptr != '%') && (*ptr != '\0'))
      ptr++;
    if (*ptr == '\0')
      fin=VERDAD;
    else if (*ptr == '\\')
      ptr+=2;
    else if (*ptr == '"')
    {
      ptr++;
      while ((*ptr != '"') && (*ptr != '\0'))
	if (*ptr == '\\')
	  ptr+=2;
	else
          ptr++;
      if (*ptr == '\0')
	fin=VERDAD;
      else
        ptr++;
    }
    else
    {
      *ptr='\0';
      fin=VERDAD;
    }
  }
}
/*----------------------------------------------------------------------------*/
char *ObtenLinea(FILE *fd, char *linea, int max)
{
  int  c;
  int  i,comillas,comentario;

  i=0;
  comillas=FALSO;
  comentario=FALSO;
  c=getc(fd);
  while ((i < max) && (c != NLINEA) && (c != EOF))
  {
    if (c == (int) '"')
    {
      if (comillas)
        comillas=FALSO;
      else
        comillas=VERDAD;
    }
    else if (c == (int) '%')
    {
      if (!comillas)
	comentario=VERDAD;
    }
    else if (c == (int) '\\')
      if (!comentario)
      {
        linea[i]=(char) c;
        i++;
        c=getc(fd);
      }
    linea[i]=(char) c;
    i++;
    c=getc(fd);
  }

  if (i >= max)
  {
    linea[max-1]='\0';
    fprintf(stderr,"\nAVISO: Linea mas larga de %d caracteres. Recompilar incrementado la constante maxima.\n",max);
    EliminaComentario(linea);
    return(linea);
  }
  else if (c == NLINEA)
  {
    linea[i]='\0';
    EliminaComentario(linea);
    return(linea);
  }
  else
  {
    ungetc(c,fd); 
    linea[i]='\0';
    EliminaComentario(linea);
    return(NULL);
  }
}
/*----------------------------------------------------------------------------*/
int LeerString(char *linea1, char *linea2, int *pi)
{
  int j,error;

  error=1;
  j=0;

  while ((linea2[*pi] == BLANCO) || (linea2[*pi] == TABULA) || (linea2[*pi] == NLINEA))
    (*pi)++;

  while ( (linea2[*pi] != BLANCO) && (linea2[*pi] != TABULA) && (linea2[*pi] != NLINEA) &&
	  (linea2[*pi] != (char)'=')    && (linea2[*pi] != (char)'\0') )
    if (linea2[*pi] == (char)'\\')
    {
      (*pi)++;
      if ( (linea2[*pi] == BLANCO) || (linea2[*pi] == TABULA) || (linea2[*pi] == (char)'=') ||
           (linea2[*pi] == (char)'"')    || (linea2[*pi] == (char)'%')    || (linea2[*pi] == (char)'\\') )
      {
        linea1[j]=linea2[*pi];
        (*pi)++;
        j++;
      }
      else if (linea2[*pi] == NLINEA)
      {
	(*pi)++;
	if (j == 0)
          while ((linea2[*pi] == BLANCO) || (linea2[*pi] == TABULA) || (linea2[*pi] == NLINEA))
            (*pi)++;
      }
    }
    else if (linea2[*pi] == (char)'"')
    {
      (*pi)++;
      while ((linea2[*pi] != (char)'"') && (linea2[*pi] != (char)'\0'))
      {
	if ( (linea2[*pi] == (char)'\\') && (linea2[*pi+1] == NLINEA) )
	  (*pi)+=2;
	else
	{
	  if ( (linea2[*pi] == (char)'\\') && ((linea2[*pi+1] == (char)'"') || (linea2[*pi+1] == (char)'\\')) )
            (*pi)++;
          linea1[j]=linea2[*pi];
          (*pi)++;
          j++;
	}
      }
      if (linea2[*pi] == (char)'\0')
	error=-35;
      else
      {
        linea1[j]=(char)'\0';
        (*pi)++;
      }
    }
    else
    {
      linea1[j]=linea2[*pi];
      (*pi)++;
      j++;
    }

  if ((j == 0) && (linea2[*pi] == (char)'='))
  {
    linea1[j]=linea2[*pi];
    (*pi)++;
    j++;
  }
  linea1[j]=(char)'\0';
  return(error);
}
/*----------------------------------------------------------------------------*/
int LineaEnBlanco(char *linea)
{
  int i;

  i=0;
  while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
    i++;
  if (linea[i] == '\0')
    return(VERDAD);
  else
    return(FALSO);
}
/*----------------------------------------------------------------------------*/
int LeerNombre(char *linea, char *auxlinea)
{
  int  i,error,auxerror;
  char *p;

  *auxlinea='\0';

  error=0;
  i=0;
  while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
    i++;
  if (linea[i] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&i);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if (strcmp(auxlinea,NOMBRE) == 0)
    {
      while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
        i++;
      if (linea[i] == '\0')
      {
	error=-1;
        auxlinea[0]='\0';
      }
      else
      {
	auxerror=LeerString(auxlinea,linea,&i);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
        while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
          i++;
        if (linea[i] != '\0')
	  error=-2;
	else
	  error=SI_NOMBRE;
      }
    }
    else
      auxlinea[0]='\0';
  }
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerNumEstados(char *linea, int *ptr_est)
{
  char *auxlinea,*p;
  int  i,error,auxerror;

  *ptr_est=NULO;
  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';

  error=0;
  i=0;
  while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
    i++;
  if (linea[i] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&i);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if (strcmp(auxlinea,NUMEST) == 0)
    {
      while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
        i++;
      if (linea[i] == '\0')
	error=-3;
      else
      {
	auxerror=LeerString(auxlinea,linea,&i);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
	*ptr_est=atoi(auxlinea);
        while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
          i++;
        if (linea[i] != '\0')
	  error=-4;
	else
	  if (*ptr_est == 0)
	    error=-5;
	  else
	    error=SI_NUMEST;
      }
    }
  }
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerNumAristas(char *linea, int *ptr_ari)
{
  char *auxlinea,*p;
  int  i,error,auxerror;

  *ptr_ari=NULO;
  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';

  error=0;
  i=0;
  while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
    i++;
  if (linea[i] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&i);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if (strcmp(auxlinea,NUMEDG) == 0)
    {
      while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
        i++;
      if (linea[i] == '\0')
	error=-6;
      else
      {
	auxerror=LeerString(auxlinea,linea,&i);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
	*ptr_ari=atoi(auxlinea);
        while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
          i++;
        if (linea[i] != '\0')
	  error=-7;
	else
	  /*
	  if (*ptr_ari == 0)
	    error=-8;
	  else
	  */
	    error=SI_NUMEDG;
      }
    }
  }
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerTotalChecksum(char *linea, double *ptr_tchk)
{
  char    *auxlinea,*p;
  int     i,error,auxerror,al,numero;

  *ptr_tchk=(double) NULO;
  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';

  error=0;
  i=0;
  while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
    i++;
  if (linea[i] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&i);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if (strcmp(auxlinea,TOTCHK) == 0)
    {
      while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
        i++;
      if (linea[i] == '\0')
	error=-9;
      else
      {
	auxerror=LeerString(auxlinea,linea,&i);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
        while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
          i++;
        if (linea[i] != '\0')
	  error=-10;
	else
	{
	  al=-1;
	  numero=VERDAD;
	  while (auxlinea[++al] != '\0')
	    numero = (numero && (((auxlinea[al] >= '0') &&
		      (auxlinea[al] <= '9')) || (auxlinea[al] == '.') ||
		      (auxlinea[al] == 'E')  || (auxlinea[al] == 'e') ||
		      (auxlinea[al] == '+')  || (auxlinea[al] == '-')));
          if (!numero)
	    error=-11;
          else
	  {
            *ptr_tchk=atof(auxlinea);
	    error=SI_TOTCHK;
          }
        }
      }
    }
  }
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerChecksum(double *checksum, char *linea, int *pi)
{
  int k,error,auxerror,al,numero;
  char *auxlinea,*p;

  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';

  error=0;
  k = *pi;

  while ((linea[k] == BLANCO) || (linea[k] == TABULA) || (linea[k] == NLINEA))
    k++;
  if (linea[k] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&k);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if ((strcmp(auxlinea,CHKSUM) == 0) || (strcmp(auxlinea,CHKSUMA) == 0))
    {
      auxerror=LeerString(auxlinea,linea,&k);
      if (auxerror < 0)
      {
        fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
        fprintf(stderr,"%s\n",linea);
        free((void *)p);
      }
      if (strcmp(auxlinea,"=") != 0)
	error=-13;
      else 
      {
	auxerror=LeerString(auxlinea,linea,&k);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
	al=-1;
	numero=VERDAD;
	while (auxlinea[++al] != '\0')
	  numero = (numero && (((auxlinea[al] >= '0') &&
		    (auxlinea[al] <= '9')) || (auxlinea[al] == '.') ||
		    (auxlinea[al] == 'E')  || (auxlinea[al] == 'e') ||
		    (auxlinea[al] == '+')  || (auxlinea[al] == '-')));
        if (!numero)
	  error=-14;
        else
	{
          *checksum=atof(auxlinea);
	  error=SI_CHK_EST;
        }
      }
      *pi=k;
    }
    else if ( (strcmp(auxlinea,INITIA) != 0) && (strcmp(auxlinea,INITIAA) != 0) &&
	      (strcmp(auxlinea,FINALE) != 0) && (strcmp(auxlinea,FINALEA) != 0) &&
	      (strcmp(auxlinea,OUTPUT) != 0) && (strcmp(auxlinea,OUTPUTA) != 0) )
    {
      *pi=k;
      error=-12;
    }
  }
  else
    *pi=k;
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerInitial(double *initial, char *linea, int *pi)
{
  int k,error,auxerror,al,numero;
  char *auxlinea,*p;

  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';

  error=0;
  k = *pi;

  while ((linea[k] == BLANCO) || (linea[k] == TABULA) || (linea[k] == NLINEA))
    k++;
  if (linea[k] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&k);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if ((strcmp(auxlinea,INITIA) == 0) || (strcmp(auxlinea,INITIAA) == 0))
    {
      auxerror=LeerString(auxlinea,linea,&k);
      if (auxerror < 0)
      {
        fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
        fprintf(stderr,"%s\n",linea);
        free((void *)p);
      }
      if (strcmp(auxlinea,"=") != 0)
	error=-15;
      else 
      {
	auxerror=LeerString(auxlinea,linea,&k);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
	al=-1;
	numero=VERDAD;
	while (auxlinea[++al] != '\0')
	  numero = (numero && (((auxlinea[al] >= '0') &&
		    (auxlinea[al] <= '9')) || (auxlinea[al] == '.') ||
		    (auxlinea[al] == 'E')  || (auxlinea[al] == 'e') ||
		    (auxlinea[al] == '+')  || (auxlinea[al] == '-')));
        if (!numero)
	  error=-46;
        else
	{
          *initial=atof(auxlinea);
	  error=SI_INI_EST;
        }
      }
      *pi=k;
    }
    else if ( (strcmp(auxlinea,CHKSUM) != 0) && (strcmp(auxlinea,CHKSUMA) != 0) &&
              (strcmp(auxlinea,FINALE) != 0) && (strcmp(auxlinea,FINALEA) != 0) &&
	      (strcmp(auxlinea,OUTPUT) != 0) && (strcmp(auxlinea,OUTPUTA) != 0) )
    {
      *pi=k;
      error=-12;
    }
  }
  else
    *pi=k;
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerFinal(double *final, char *linea, int *pi)
{
  int k,error,auxerror,al,numero;
  char *auxlinea,*p;

  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';

  error=0;
  k = *pi;

  while ((linea[k] == BLANCO) || (linea[k] == TABULA) || (linea[k] == NLINEA))
    k++;
  if (linea[k] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&k);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if ((strcmp(auxlinea,FINALE) == 0) || (strcmp(auxlinea,FINALEA) == 0))
    {
      auxerror=LeerString(auxlinea,linea,&k);
      if (auxerror < 0)
      {
        fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
        fprintf(stderr,"%s\n",linea);
        free((void *)p);
      }
      if (strcmp(auxlinea,"=") != 0)
	error=-16;
      else 
      {
	auxerror=LeerString(auxlinea,linea,&k);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
	al=-1;
	numero=VERDAD;
	while (auxlinea[++al] != '\0')
	  numero = (numero && (((auxlinea[al] >= '0') &&
		    (auxlinea[al] <= '9')) || (auxlinea[al] == '.') ||
		    (auxlinea[al] == 'E')  || (auxlinea[al] == 'e') ||
		    (auxlinea[al] == '+')  || (auxlinea[al] == '-')));
        if (!numero)
	  error=-47;
        else
	{
          *final=atof(auxlinea);
	  error=SI_FIN_EST;
        }
      }
      *pi=k;
    }
    else if ( (strcmp(auxlinea,CHKSUM) != 0) && (strcmp(auxlinea,CHKSUMA) != 0) &&
              (strcmp(auxlinea,INITIA) != 0) && (strcmp(auxlinea,INITIAA) != 0) &&
              (strcmp(auxlinea,OUTPUT) != 0) && (strcmp(auxlinea,OUTPUTA) != 0) )
    {
      *pi=k;
      error=-12;
    }
  }
  else
    *pi=k;
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerOutput(char *output, char *linea, int *pi)
{
  int k,error,auxerror;
  char *auxlinea,*p;

  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';

  error=0;
  k = *pi;

  while ((linea[k] == BLANCO) || (linea[k] == TABULA) || (linea[k] == NLINEA))
    k++;
  if (linea[k] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&k);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if ((strcmp(auxlinea,OUTPUT) == 0) || (strcmp(auxlinea,OUTPUTA) == 0))
    {
      auxerror=LeerString(auxlinea,linea,&k);
      if (auxerror < 0)
      {
        fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
        fprintf(stderr,"%s\n",linea);
        free((void *)p);
      }
      if (strcmp(auxlinea,"=") != 0)
	error=-17;
      else 
      {
	auxerror=LeerString(auxlinea,linea,&k);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
	if ((linea[k-1] != '"') && (Reservada(auxlinea)))
	  error=-18;
	else
	{
          strcpy(output,auxlinea);
          error=SI_OUT_E_A;
	}
      }
      *pi=k;
    }
    else if ( (strcmp(auxlinea,CHKSUM) != 0) && (strcmp(auxlinea,CHKSUMA) != 0) &&
	      (strcmp(auxlinea,INITIA) != 0) && (strcmp(auxlinea,INITIAA) != 0) &&
              (strcmp(auxlinea,FINALE) != 0) && (strcmp(auxlinea,FINALEA) != 0) &&
	      (strcmp(auxlinea,PROBAB) != 0) && (strcmp(auxlinea,PROBABA) != 0) )
    {
      *pi=k;
      error=-12;
    }
  }
  else
    *pi=k;
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerEstado(char *linea, char *ident, double *checksum, double *initial,
	       double *final, char *output)
{
  char *auxlinea,*p;
  int  i,error,auxerror,kk=0;

  int bchecksum=FALSO,binitial=FALSO,bfinal=FALSO,boutput=FALSO;

  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';
  error=0;

  i=0;
  while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
    i++;
  if (linea[i] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&i);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if (strcmp(auxlinea,ESTADO) == 0)
    {
      while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
        i++;
      if (linea[i] == '\0')
	error=-19;
      else
      {
        auxerror=LeerString(auxlinea,linea,&i);
        if (auxerror < 0)
        {
          fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
          free((void *)p);
        }
/*
        if ((linea[i-1] != '"') && (Reservada(auxlinea)))
	  error=-20;
	else
	{
*/
	  strcpy(ident,auxlinea);
	  error=NO_OPC_EST;

          while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
            i++;
	  *checksum = 0.0;
	  *initial = 0.0;
	  *final = 0.0;
	  *output = '\0';
	  while (linea[i] != '\0')
	  {
            if ((auxerror=LeerChecksum(checksum,linea,&i)) == SI_CHK_EST)
	    {
	      if (!bchecksum)
	      {
		bchecksum = VERDAD;
		if (error == NO_OPC_EST)
		  error=SI_CHK_EST;
		else
		  error+=SI_CHK_EST;
	      }
              else
	      {
	        fprintf(stderr,"\nAVISO: %s\n",p=GetError(-21));
                fprintf(stderr,"%s\n",linea);
		free((void *)p);
	      }
	    }
	    else if (auxerror < 0)
	    {
	      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
              fprintf(stderr,"%s\n",linea);
	      free((void *)p);
	    }

            if ((auxerror=LeerInitial(initial,linea,&i)) == SI_INI_EST)
	    {
	      if (!binitial)
	      {
		binitial = VERDAD;
		if (error == NO_OPC_EST)
		  error=SI_INI_EST;
		else
		  error+=SI_INI_EST;
	      }
	      else
	      {
	        fprintf(stderr,"\nAVISO: %s\n",p=GetError(-22));
                fprintf(stderr,"%s\n",linea);
		free((void *)p);
	      }
	    }
	    else if (auxerror < 0)
	    {
	      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
              fprintf(stderr,"%s\n",linea);
	      free((void *)p);
	    }

            if ((auxerror=LeerFinal(final,linea,&i)) == SI_FIN_EST)
	    {
	      if (!bfinal)
	      {
		bfinal = VERDAD;
		if (error == NO_OPC_EST)
		  error=SI_FIN_EST;
		else
		  error+=SI_FIN_EST;
	      }
	      else
	      {
	        fprintf(stderr,"\nAVISO: %s\n",p=GetError(-23));
                fprintf(stderr,"%s\n",linea);
		free((void *)p);
	      }
	    }
	    else if (auxerror < 0)
	    {
	      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
              fprintf(stderr,"%s\n",linea);
	      free((void *)p);
	    }

            if ((auxerror=LeerOutput(output,linea,&i)) == SI_OUT_E_A)
	    {
	      if (!boutput)
	      {
		boutput = VERDAD;
		if (error == NO_OPC_EST)
		  error=SI_OUT_E_A;
		else
		  error+=SI_OUT_E_A;
	      }
	      else
	      {
	        fprintf(stderr,"\nAVISO: %s\n",p=GetError(-24));
                fprintf(stderr,"%s\n",linea);
		free((void *)p);
	      }
	    }
	    else if (auxerror < 0)
	    {
	      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
              fprintf(stderr,"%s\n",linea);
	      free((void *)p);
	    }
	  }
/*
        }
*/
      }
    }
  }
  free((void *)auxlinea);

  if (kk) {
    fprintf(stderr,"%d ",auxerror);
    fprintf(stderr,"%d\n",error);
  }

  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerProbabilidad(double *prob, char *linea, int *pi)
{
  int k,error,auxerror,al,numero;
  char *auxlinea,*p;

  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';

  error=0;
  k = *pi;

  while ((linea[k] == BLANCO) || (linea[k] == TABULA) || (linea[k] == NLINEA))
    k++;
  if (linea[k] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&k);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
    if ((strcmp(auxlinea,PROBAB) == 0) || (strcmp(auxlinea,PROBABA) == 0))
    {
      auxerror=LeerString(auxlinea,linea,&k);
      if (auxerror < 0)
      {
        fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
        fprintf(stderr,"%s\n",linea);
        free((void *)p);
      }
      if (strcmp(auxlinea,"=") != 0)
	error=-25;
      else 
      {
	auxerror=LeerString(auxlinea,linea,&k);
	if (auxerror < 0)
	{
	  fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
	  free((void *)p);
	}
	al=-1;
	numero=VERDAD;
	while (auxlinea[++al] != '\0')
	  numero = (numero && (((auxlinea[al] >= '0') &&
		    (auxlinea[al] <= '9')) || (auxlinea[al] == '.') ||
		    (auxlinea[al] == 'E')  || (auxlinea[al] == 'e') ||
		    (auxlinea[al] == '+')  || (auxlinea[al] == '-')));
        if (!numero)
	  error=-26;
        else
	{
          *prob=atof(auxlinea);
	  error=SI_PRO_ARI;
        }
      }
      *pi=k;
    }
    else if ( (strcmp(auxlinea,OUTPUT) != 0) && (strcmp(auxlinea,OUTPUTA) != 0) )
    {
      *pi=k;
      error=-12;
    }
  }
  else
    *pi=k;
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
int LeerArista(char *linea, char *origen, char *destino, char *simbolo,
	       double *prob, char *output)
{
  char *auxlinea,*p;
  int  i,error,auxerror;
  int bprob=FALSO,boutput=FALSO;

  auxlinea=ASIGMEM(char,MAXLIN);
  *auxlinea='\0';
  error=0;

  i=0;
  while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
    i++;
  if (linea[i] != '\0')
  {
    auxerror=LeerString(auxlinea,linea,&i);
    if (auxerror < 0)
    {
      fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
      fprintf(stderr,"%s\n",linea);
      free((void *)p);
    }
/*
    if ((linea[i-1] != '"') && (Reservada(auxlinea)))
      error=-29;
    else
    {
*/
      strcpy(origen,auxlinea);
      *auxlinea='\0';
      while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
        i++;

      if (linea[i] == '\0')
	error=-30;
      else
      {
        auxerror=LeerString(auxlinea,linea,&i);
        if (auxerror < 0)
        {
          fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
          fprintf(stderr,"%s\n",linea);
          free((void *)p);
        }
/*
        if ((linea[i-1] != '"') && (Reservada(auxlinea)))
	  error=-31;
        else
        {
*/
          strcpy(destino,auxlinea);
          *auxlinea='\0';
          while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
            i++;

          if (linea[i] == '\0')
	    error=-30;
	  else
	  {
            auxerror=LeerString(auxlinea,linea,&i);
            if (auxerror < 0)
            {
              fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
              fprintf(stderr,"%s\n",linea);
              free((void *)p);
            }
/*
            if ((linea[i-1] != '"') && (Reservada(auxlinea)))
	      error=-32;
            else
            {
*/
              strcpy(simbolo,auxlinea);
	      error=NO_OPC_ARI;
              while ((linea[i] == BLANCO) || (linea[i] == TABULA) || (linea[i] == NLINEA))
                i++;

              *prob = 0.0;
              *output = '\0';
              while (linea[i] != '\0')
              {
                if ((auxerror=LeerProbabilidad(prob,linea,&i)) == SI_PRO_ARI)
                {
                  if (!bprob)
		  {
                    bprob = VERDAD;
		    if (error == NO_OPC_ARI)
		      error=SI_PRO_ARI;
	            else
		      error+=SI_PRO_ARI;
		  }
                  else
	          {
	            fprintf(stderr,"\nAVISO: %s\n",p=GetError(-33));
                    fprintf(stderr,"%s\n",linea);
		    free((void *)p);
	          }
                }
	        else if (auxerror < 0)
	        {
	          fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
                  fprintf(stderr,"%s\n",linea);
		  free((void *)p);
	        }

                if ((auxerror=LeerOutput(output,linea,&i)) == SI_OUT_E_A)
                {
                  if (!boutput)
		  {
                    boutput = VERDAD;
		    if (error == NO_OPC_ARI)
		      error=SI_OUT_E_A;
	            else
		      error+=SI_OUT_E_A;
		  }
                  else
	          {
	            fprintf(stderr,"\nAVISO: %s\n",p=GetError(-24));
                    fprintf(stderr,"%s\n",linea);
		    free((void *)p);
	          }
                }
	        else if (auxerror < 0)
	        {
	          fprintf(stderr,"\nAVISO: %s\n",p=GetError(auxerror));
                  fprintf(stderr,"%s\n",linea);
		  free((void *)p);
	        }

              }
/*
            }
*/
	  }
/*
        }
*/
      }
/*
    }
*/
  }
  free((void *)auxlinea);
  return(error);
}
/*----------------------------------------------------------------------------*/
void DivideEnLineas(char *cadena1, char *cadena2)
{
  char *p,*q;
  int  i;

  p=cadena1;
  q=cadena2;
  i=0;
  while (*p != '\0')
  {
    if (i <80)
    {
      *q=*p; i++; p++; q++;
    }
    else
    {
      i=0;
      q--; p--;
      while ((*q != BLANCO) && (*q != TABULA) && (i < 20))
      {
	q--; p--; i++;
      }
      if (i == 20)
      {
        q+=20; p+=20;
      }
      *q='\\';
      q++;
      *q='\n';
      q++;
      i=0;
    }
  }
  *q='\0';
}
/*----------------------------------------------------------------------------*/
void EscribeNombre(char *linea, char *nombre)
{
  sprintf(linea,"%s   %s\n",NOMBRE,nombre);
}
/*----------------------------------------------------------------------------*/
void EscribeNumEstados(char *linea, int est)
{
  sprintf(linea,"%s       %d\n",NUMEST,est);
}
/*----------------------------------------------------------------------------*/
void EscribeNumAristas(char *linea, int ari)
{
  sprintf(linea,"%s        %d\n",NUMEDG,ari);
}
/*----------------------------------------------------------------------------*/
void EscribeTotalChecksum(char *linea, double tchk, int tipored)
{
  int entero;

  if ( (tipored == AUTO_PROB) || (tipored == TRAN_PROB) )
  {
    entero= (int) tchk;
    if (tchk == (double) entero)
      sprintf(linea,"%s   %d\n",TOTCHK,entero);
    else
      sprintf(linea,"%s   %G\n",TOTCHK,tchk);
  }
  else
    linea[0]='\0';
}
/*----------------------------------------------------------------------------*/
void EscribeEstado(char *linea, int ident, double checksum, double initial,
	      double final, char *output, int tipored)
{
  char  auxlinea[MAXLIN];
  int   entero;

  sprintf(linea,"%s   %d ",ESTADO,ident);

  if ( (tipored == AUTO_PROB) || (tipored == TRAN_PROB) )
  {
    entero= (int) checksum;
    if (checksum == (double) entero)
      sprintf(auxlinea,"   %s = %d",CHKSUMA,entero);
    else
      sprintf(auxlinea,"   %s = %G",CHKSUMA,checksum);
    strcat(linea,auxlinea);
  }

  if (initial > 0.0)
  {
    if ( (tipored == AUTO_PROB) || (tipored == TRAN_PROB) )
    {
      entero= (int) initial;
      if (initial == (double) entero)
        sprintf(auxlinea,"   %s = %d",INITIAA,entero);
      else
        sprintf(auxlinea,"   %s = %G",INITIAA,initial);
    }
    else
      sprintf(auxlinea,"   %s = 1",INITIAA);
    strcat(linea,auxlinea);
  }

  if (final > 0.0)
  {
    if ( (tipored == AUTO_PROB) || (tipored == TRAN_PROB) )
    {
      entero= (int) final;
      if (final == (double) entero)
        sprintf(auxlinea,"   %s = %d",FINALEA,entero);
      else
        sprintf(auxlinea,"   %s = %G",FINALEA,final);
    }
    else
      sprintf(auxlinea,"   %s = 1",FINALEA);
    strcat(linea,auxlinea);
  }

  /* if ( ((tipored == TRANSDUCTOR) || (tipored == TRAN_PROB))
     && (tipoest == CON_OUTPUT) ) */

  if ((output != NULL) && (*output != '\0')) {
   sprintf(auxlinea,"   %s = \"%s\"",OUTPUTA,output);
   strcat(linea,auxlinea);
  } 

  strcat(linea,"\n");
  if (linea80) {
    DivideEnLineas(linea,auxlinea);
    strcpy(linea,auxlinea);
  }
}
/*----------------------------------------------------------------------------*/
void EscribeArista(char *linea, int origen, int destino, char *simbolo,
	           double prob, char *output, int tipored)
{
  char  auxlinea[MAXLIN];
  int   entero;

  sprintf(linea,"%d   %d   %s ",origen,destino,simbolo);

  if ( (tipored == AUTO_PROB) || (tipored == TRAN_PROB) )
  {
    entero= (int) prob;
    if (prob == (double) entero)
      sprintf(auxlinea,"   %s = %-10d",PROBABA,entero);
    else
      sprintf(auxlinea,"   %s = %-10G",PROBABA,prob);
    strcat(linea,auxlinea);
  }

  if ( (tipored == TRANSDUCTOR) || (tipored == TRAN_PROB) )
  {
    sprintf(auxlinea,"   %s = \"%s\"",OUTPUTA,output);
    strcat(linea,auxlinea);
  }

  strcat(linea,"\n");
  if (linea80) {
    DivideEnLineas(linea,auxlinea);
    strcpy(linea,auxlinea);
  }
}
/*----------------------------------------------------------------------------*/
void Lineas80Caracteres(void)
{
  linea80=VERDAD;
}
/*----------------------------------------------------------------------------*/
