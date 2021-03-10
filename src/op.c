/*
 *  optparser.c - Implementació de la classe de la llibreria «op».
 *
 *  Adrià Giménez Pastor, 2006. <agimenez@dsic.upv.es>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */


#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "op.h"


/**********/
/* MACROS */
/**********/

#define FALSE 0
#define TRUE  1


#define T_INT    0
#define T_FLOAT  1
#define T_DOUBLE 2
#define T_ENUM   3
#define T_STRING 4
#define T_COUNT  5
#define T_FLAG   6


#define INDEX2SNAME(i) ( (i >= 0 && i < 26) ? i + 'a' : i - 26 + 'A' )
#define SNAME2INDEX(c) ( (c >= 'a' && c <= 'z') ? c - 'a' : c - 'A' + 26 )




/********************************/
/* DECLARACIÓ FUNCIONS PRIVADES */
/********************************/

static int
__sname2index (
	       const char c
	       );

static struct __op_node_value *
__add_option (
	      op_parser_t    *parser,
	      const char      shortopt,
	      const char     *longopt,
	      const char     *metavar,
	      const char     *help
	      );




/************/
/* FUNCIONS */
/************/

op_parser_t *
op_create (
	   const char     *usage,
	   const char     *version,
	   int             unks
	   )
{

  /* VARS */

  op_parser_t* newc;
  int i;
  
  
  /* CODE */
  
  if ( (newc= (op_parser_t *) malloc ( sizeof (op_parser_t) )) == NULL )
    return NULL;
  
  /* INIT TO NULL ALL POINTER ARGS. */
  
  newc->__lopts= NULL;
  
  
  /* INIT OBJECT. */
  
  if ( usage == NULL || version == NULL )
    goto error;
  
  newc->__usage= usage;
  newc->__version= version;
  newc->__unks= unks;
  newc->__max_length= 7;
  newc->__prog= "???";
  newc->__lopts2= NULL;
  
  for ( i= 0; i < 52; i++ )
    newc->__sopts[i]= NULL;
  
  return newc;
  
  
 error:
  
  op_destroy ( newc );
  return NULL;

} /* end op_create */


void
op_destroy (
	    op_parser_t *parser
	    )
{
  
  struct __op_node *p, *fem;
  
  
  /* FREE MEM FROM ARGS & ACTIONS. */
  
  p= parser->__lopts;
  while ( p != NULL )
    {
      fem= p;
      p= p->next;
      free ( fem );
    }
  
  free ( parser );

} /* end op_destroy */




void
op_add_int (
	    op_parser_t    *parser,
	    const char      shortopt,
	    const char     *longopt,
	    const char     *metavar,
	    const char     *help,
	    int            *dest,
	    int             defaultv
	    )
{

  /* VARS */
  
  struct __op_node_value *value;
  
  
  /* CODE */
  
  value= __add_option ( parser, shortopt, longopt,
			metavar, help );
  value->type= T_INT;
  value->dest.i= dest;
  *dest= defaultv;
  
} /* end op_add_int */


void
op_add_float (
	      op_parser_t    *parser,
	      const char      shortopt,
	      const char     *longopt,
	      const char     *metavar,
	      const char     *help,
	      float          *dest,
	      float           defaultv
	      )
{

  /* VARS */
  
  struct __op_node_value *value;
  
  
  /* CODE */
  
  value= __add_option ( parser, shortopt, longopt,
			metavar, help );
  value->type= T_FLOAT;
  value->dest.f= dest;
  *dest= defaultv;
  
} /* end op_add_float */


void
op_add_double (
	       op_parser_t    *parser,
	       const char      shortopt,
	       const char     *longopt,
	       const char     *metavar,
	       const char     *help,
	       double         *dest,
	       double          defaultv
	       )
{

  /* VARS */
  
  struct __op_node_value *value;
  
  
  /* CODE */
  
  value= __add_option ( parser, shortopt, longopt,
			metavar, help );
  value->type= T_DOUBLE;
  value->dest.d= dest;
  *dest= defaultv;
  
} /* end op_add_double */


void
op_add_enum (
	     op_parser_t     *parser,
	     const char       shortopt,
	     const char      *longopt,
	     const char      *metavar,
	     const char      *help,
	     const char     **list,
	     int             *dest,
	     int              defaultv
	     )
{

  /* VARS */
  
  struct __op_node_value *value;
  
  
  /* CODE */
  
  value= __add_option ( parser, shortopt, longopt,
			metavar, help );
  value->type= T_ENUM;
  value->dest.e.list= list;
  value->dest.e.index= dest;
  *dest= defaultv;
  
} /* end op_add_enum */


void
op_add_string (
	       op_parser_t     *parser,
	       const char       shortopt,
	       const char      *longopt,
	       const char      *metavar,
	       const char      *help,
	       char           **dest,
	       const char      *defaultv
	       )
{

  /* VARS */
  
  struct __op_node_value *value;
  
  
  /* CODE */
  
  value= __add_option ( parser, shortopt, longopt,
			metavar, help );
  value->type= T_STRING;
  value->dest.s= dest;
  *dest= (char *) defaultv;
  
} /* end op_add_string */


void
op_add_counter (
		op_parser_t     *parser,
		const char       shortopt,
		const char      *longopt,
		const char      *help,
		int             *dest
		)
{

  /* VARS */
  
  struct __op_node_value *value;
  
  
  /* CODE */
  
  value= __add_option ( parser, shortopt, longopt,
			"", help );
  value->type= T_COUNT;
  value->dest.c= dest;
  *dest= 0;
  
} /* end op_add_counter */


void
op_add_flag (
	     op_parser_t     *parser,
	     const char       shortopt,
	     const char      *longopt,
	     const char      *help,
	     int             *dest
	     )
{

  /* VARS */
  
  struct __op_node_value *value;
  
  
  /* CODE */
  
  value= __add_option ( parser, shortopt, longopt,
			"", help );
  value->type= T_FLAG;
  value->dest.fl= dest;
  *dest= FALSE;
  
} /* end op_add_flag */


void
op_usage (
	  op_parser_t    *parser,
	  FILE           *stream
	  )
{
  
  /* VARS */
  
  int i, init, j;
  int desp;
  char c;
  struct __op_node *p;
  
  
  /* CODE */
  
  /* Imprimeix cadena d'ús. */
  fprintf ( stream, "\nUsage: " );
  i= 0;
  while ( parser->__usage[i] != '\0' )
    {
      
      if ( parser->__usage[i] == '%' &&
	   parser->__usage[i+1] == 'p' )
	{
	  fputs ( parser->__prog, stream );
	  ++i;
	}
      else
	fputc ( parser->__usage[i], stream );
      
      ++i;
      
    }
  fputc ( '\n', stream );
  
  /* Imprimeix opcions. */
  desp= parser->__max_length + 11;
  fprintf ( stream, "\noptions:\n\n" );
  
  /* Imprimeix opcions per defecte. */
  fprintf ( stream, "  -V, --version" );
  for ( i= 15; i < desp; i++ ) fputc ( ' ', stream );
  fprintf ( stream, "output version information and exit\n" );
  
  fprintf ( stream, "  -h, --help" );
  for ( i= 12; i < desp; i++ ) fputc ( ' ', stream );
  fprintf ( stream, "display this help and exit\n" );
  
  /* Imprimeix reste d'opcions. */
  p= parser->__lopts;
  while ( p != NULL )
    {
   
      init= p->length + 8;
      fprintf ( stream, "  -%c, --%s", INDEX2SNAME ( p->sindex ), p->name );
      if ( p->value.type != T_FLAG &&
	   p->value.type != T_COUNT )
	{
	  fprintf ( stream, "=%s", p->value.metavar );
	  ++init;
	}
      for ( i= init; i < desp; i++ ) fputc ( ' ', stream );
      i= 0;
      while ( (c= p->value.help[i]) != '\0' )
	{
	  if ( c != '\n' )
	    fputc ( c, stream );
	  else
	    {
	      fputc ( c, stream );
	      for ( j= 0; j < desp; j++ )
		fputc ( ' ', stream );
	    }
	  ++i;
	}
      fputc ( '\n', stream );
      
      p= p->next;
      
    }
  fputc ( '\n', stream );
  
} /* end op_usage */


int
op_parse (
	  op_parser_t    *parser,
	  int             argc,
	  char           *argv[]
	  )
{
  
  /* VARS */
  
  int i, j, k, n, index;
  int sindex, invalid, end_args;
  char c;
  char *lname;
  struct __op_node_value *value;
  struct __op_node *prev, *p;
  
  
  /* CODE */
  
  /* Llig el nom del programa. */
  parser->__prog= argv[0];
  
  /* Parseja les opcions i arguments. */
  i= 0;
  j= 1;
  end_args= FALSE;
  while ( j < argc )
    {
      
      /* Si és un argument. */
      if ( argv[j][0] != '-' || end_args )
	argv[i++]= argv[j];
      
      /* Fi arguments. */
      else if ( argv[j][1] == '-' &&
		argv[j][2] == '\0' )
	end_args= TRUE;
      
      /* És una posible opció. */
      else
	{
	  
	  /* Inicialització. */
	  c= argv[j][1];
	  value= NULL;
	  invalid= FALSE;
	  
	  /* És una opció curta. */
	  if ( c != '-' )
	    {
	      
	      if ( c < 'A' || c > 'z' ||
		   (c > 'Z' && c < 'a') )
		invalid= TRUE;
	      else if ( c == 'h' )
		goto help;
	      else if ( c == 'V' )
		goto version;
	      else
		{
		  
		  sindex= SNAME2INDEX ( c );
		  if ( (value= parser->__sopts[sindex]) == NULL )
		    {
		      if ( !parser->__unks )
			goto unk;
		    }
		  else
		    {
		      /* Actualiza variables de posició. */
		      k= 2;
		      
		    }
		}
	      
	    }
	  
	  /* És una opció llarga. */
	  else
	    {
	      
	      /* Delilimita nom. */
	      n= 0;
	      lname= &(argv[j][2]);
	      while ( lname[n] != '\0' &&
		      lname[n] != '=' )
		++n;
	      
	      if ( n == 0 )
		invalid= TRUE;
	      else
		{
		  
		  /* Busca node. */
		  prev= NULL;
		  p= parser->__lopts2;
		  while ( p != NULL &&
			  (p->n != n ||
			   (strncmp ( p->name, lname, n ) != 0)) )
		    {
		      prev= p;
		      p= p->next2;
		    }
		  
		  /* En funció del que troba. */
		  if ( p != NULL )
		    {
		      
		      value= &(p->value);
		      
		      /* Possa el node al final. */
		      if ( value->type != T_COUNT )
			{
			  parser->__last2->next2= p;
			  parser->__last2= p;
			  if ( prev != NULL )
			    prev->next2= p->next2;
			  else
			    parser->__lopts2= p->next2;
			  p->next2= NULL;
			}
		      
		      /* Actualiza variables de posició. */
		      k= n + 2;
		      
		    }
		  else if ( n == 4 && strncmp ( lname, "help", 4 ) == 0 )
		    goto help;
		  else if ( n == 7 && strncmp ( lname, "version", 7 ) == 0 )
		    goto version;
		  else if ( !parser->__unks )
		    goto unk;
		  
		}
	      
	    }
	  
	  if ( invalid )
	    {
	      if ( !parser->__unks )
		goto inv;
	    }
	  else if ( value != NULL )
	    {
	      
	      /* Actualitza variables posició. */
	      if ( value->type != T_COUNT &&
		   value->type != T_FLAG )
		{
		  if ( argv[j][k] == '=' ) ++k;
		  if ( argv[j][k] == '\0' )
		    {
		      
		      k= 0;
		      ++j;
		      
		      /* Comprova que no ens anem de mare. */
		      if ( j == argc )
			{
			  if ( !parser->__unks )
			    {
			      --j;
			      goto inv;
			    }
			  else break;
			}
		    }
		  
		  /* Llig valors. */
		  switch ( value->type )
		    {
		      
		    case T_INT:
		      
		      *(value->dest.i)= atoi ( &(argv[j][k]) );
		      break;
		      
		    case T_FLOAT:
		      
		      *(value->dest.f)= (float) atof ( &(argv[j][k]) );
		      break;
		      
		    case T_DOUBLE:
		      
		      *(value->dest.d)= atof ( &(argv[j][k]) );
		      break;
		      
		    case T_ENUM:
		      
		      index= 0;
		      while ( value->dest.e.list[index] != NULL &&
			      (strcmp ( value->dest.e.list[index], &(argv[j][k]) ) != 0) )
			++index;
		      
		      if ( value->dest.e.list[index] == NULL )
			{
			  fprintf ( stderr, "%s: '%s' no és un valor vàlid de l'enumeració.\n",
				    parser->__prog,
				    &(argv[j][k]) );
			  op_destroy ( parser );
			  exit ( EXIT_FAILURE );
			}
		      else
			*(value->dest.e.index)= index;
		      
		      break;
		      
		    case T_STRING:
		      
		      *(value->dest.s)= &(argv[j][k]);
		      break;
		      
		    }
		}
	      else if ( argv[j][k] == '=' )
		{
		  if ( !parser->__unks )
		    goto inv;
		}
	      else if ( value->type == T_COUNT )
		++(*(value->dest.c));
	      else if ( value->type == T_FLAG )
		*(value->dest.fl)= TRUE;
	      
	    }
	  
	}
      
      ++j;
      
    }
  
  return i;
  
  
 version:
  
  printf ( "%s %s\n", parser->__prog,
	   parser->__version );
  op_destroy ( parser );
  exit ( EXIT_SUCCESS );

 help:
  
  op_usage ( parser, stdout );
  op_destroy ( parser );
  exit ( EXIT_SUCCESS );

 inv:
  
  fprintf ( stderr, "%s: '%s' no és una opció vàlida.\n",
	    parser->__prog, argv[j] );
  op_destroy ( parser );
  exit ( EXIT_FAILURE );
  
 unk:
  
  fprintf ( stderr, "%s: l'opció '%s' és desconeguda.\n",
	    parser->__prog, argv[j] );
  op_destroy ( parser );
  exit ( EXIT_FAILURE );
  
} /* end op_parse */




/*********************/
/* FUNCIONS PRIVADES */
/*********************/

static int
__sname2index (
	       const char c
	       )
{

  /* VARS */
  
  /* CODE */
  
  if ( c >= 'a' && c <= 'z' && c != 'h' )
    return (int) (c - 'a');
  else if ( c >= 'A' && c <= 'Z' && c != 'V' )
    return (int) (c - 'A' + 26);
  else
    {
      fprintf ( stderr, "[op ERROR]: nom incorrecte per a una opció curta.\n" );
      exit ( EXIT_FAILURE );
    }
  
} /* end __sname2index */


static struct __op_node_value *
__add_option (
	      op_parser_t    *parser,
	      const char      shortopt,
	      const char     *longopt,
	      const char     *metavar,
	      const char     *help
	      )
{
  
  /* VARS */
  
  struct __op_node *new_node;
  struct __op_node_value *ret;
  int sindex;
  int length;
  
  
  /* CODE */
  
  /* Comprova Nulls. */
  if ( longopt == NULL || metavar == NULL || help == NULL )
    {
      fprintf ( stderr, "[op ERROR]: alguns camps tenen valor NULL.\n" );
      exit ( EXIT_FAILURE );
    }
  
  /* Obté l'índex de l'opció curta. */
  sindex= __sname2index ( shortopt );
  
  /* Reserva memòria. */
  if ( (new_node= (struct __op_node *)
	malloc ( sizeof (struct __op_node) )) == NULL )
    {
      fprintf ( stderr, "[op ERROR]: memòria insuficient.\n" );
      exit ( EXIT_FAILURE );
    }
  
  /* Inserta nou node. */
  new_node->name= longopt;
  new_node->sindex= sindex;
  new_node->value.help= help;
  new_node->value.metavar= metavar;
  new_node->n= strlen ( longopt );
  new_node->length= new_node->n + strlen ( metavar );
  ret= &(new_node->value);
  
  new_node->next= NULL;
  if ( parser->__lopts == NULL )
      parser->__lopts= new_node;
  else
    parser->__last->next= new_node;
  parser->__last= new_node;
  
  new_node->next2= parser->__lopts2;
  if ( parser->__lopts2 == NULL )
    parser->__last2= new_node;
  parser->__lopts2= new_node;
  
  
  parser->__sopts[sindex]= ret;
  
  /* Actualitza MAX_LENGTH. */
  if ( new_node->length > parser->__max_length )
    parser->__max_length= new_node->length;
  
  return ret;
  
} /* end __add_option */
