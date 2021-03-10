/*
 *  optparser.h - Definició de la lliberia «op». Parseja els
 *                arguments d'entrada del programa obtenint el valor
 *                de les opcions especificades.
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


#ifndef __OP_H__
#define __OP_H__


#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


/* TIPUS */

typedef struct __op_node __op_node;

struct __op_node_value
{
  
  unsigned char  type;
  const char    *metavar;
  const char    *help;
  union
  {
    
    int         *i;
    float       *f;
    double      *d;
    struct
    {
      
      const char **list;
      int         *index;
      
    }            e;
    char       **s;
    int         *c;
    int         *fl;
    
  }              dest;
  
};

struct __op_node
{
  
  const char             *name;
  int                     sindex;
  int                     n;
  int                     length;
  struct __op_node_value  value;
  struct __op_node       *next;
  struct __op_node       *next2;
  
};

typedef struct __op_parser
{
  
  const char             *__usage;         /* Cadena d'ús,                 
					      %p és argv[0]. */            
  const char             *__version;       /* Cadena de versió,            
					      %p és argv[0]. */            
  struct __op_node       *__lopts;         /* Llista d'opcions             
					      llargues. */                 
  struct __op_node       *__lopts2;        /* Per a parsejar. */           
  struct __op_node_value *__sopts[52];     /* Llista d'opcions curtes*/    
  int                     __unks;          /* Permet opcions               
					      desconegudes. */             
  int                     __max_length;    /* Longitud màxima de           
					      NOM_LLARG + METAVAT. */      
  const char             *__prog;          /* Nom del programa. */         
  struct __op_node       *__last;          /* Últim de la llista. */       
  struct __op_node       *__last2;         /* Per a parsejar. */           

} op_parser_t;


/* FUNCIONS */

op_parser_t *
op_create (
	   const char     *usage,
	   const char     *version,
	   int             unks
	   );

void
op_destroy (
	    op_parser_t *parser
	    );

void
op_add_int (
	    op_parser_t *parser,
	    const char      shortopt,    /* Nom curt. */
	    const char     *longopt,     /* Nom llarg. */
	    const char     *metavar,     /* Metavar associada. */
			const char     *help,        /* Cadena d'ajuda. */
	    int            *dest,        /* Variable destí. */
	    int             defaultv     /* Valor per defecte. */
	    );

void
op_add_float (
	      op_parser_t *parser,
	      const char      shortopt,    /* Nom curt. */
	      const char     *longopt,     /* Nom llarg. */
	      const char     *metavar,     /* Metavar associada. */
	      const char     *help,        /* Cadena d'ajuda. */
	      float          *dest,        /* Variable destí. */
	      float           defaultv     /* Valor per defecte. */
	      );

void
op_add_double (
	       op_parser_t    *parser,
	       const char      shortopt,    /* Nom curt. */
	       const char     *longopt,     /* Nom llarg. */
	       const char     *metavar,     /* Metavar associada. */
	       const char     *help,        /* Cadena d'ajuda. */
	       double         *dest,        /* Variable destí. */
	       double          defaultv     /* Valor per defecte. */
	       );

void
op_add_enum (
	     op_parser_t     *parser,
	     const char       shortopt,    /* Nom curt. */
	     const char      *longopt,     /* Nom llarg. */
	     const char      *metavar,     /* Metavar associada. */
	     const char      *help,        /* Cadena d'ajuda. */
	     const char     **list,        /* Llista de tokens.
					      Amb Sentinela. */
	     int             *dest,        /* Variable destí. */
	     int              defaultv     /* Valor per defecte. */
	     );

void
op_add_string (
	       op_parser_t     *parser,
	       const char       shortopt,    /* Nom curt. */
	       const char      *longopt,     /* Nom llarg. */
	       const char      *metavar,     /* Metavar associada. */
	       const char      *help,        /* Cadena d'ajuda. */
	       char           **dest,        /* Variable destí. */
	       const char      *defaultv     /* Valor per defecte. */
	       );

void
op_add_counter (
		op_parser_t     *parser,
		const char       shortopt,    /* Nom curt. */
		const char      *longopt,     /* Nom llarg. */
		const char      *help,        /* Cadena d'ajuda. */
		int             *dest         /* Variable destí. */
		);

void
op_add_flag (
	     op_parser_t     *parser,
	     const char       shortopt,    /* Nom curt. */
	     const char      *longopt,     /* Nom llarg. */
	     const char      *help,        /* Cadena d'ajuda. */
	     int             *dest         /* Variable destí. */
	     );

void
op_usage (
	  op_parser_t    *parser,
	  FILE           *stream    /* Fluxe on imprimir. */
	  );

int    /* Torna el nombre d'arguments. */
op_parse (
	  op_parser_t    *parser,
	  int             argc,     /* Nombre d'arguments. */
	  char           *argv[]    /* Arguments. */
	  );

#ifdef __cplusplus
}
#endif

#endif /* __OP_H__ */
