#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>
/* #include "giati.h" */

/* #include <libs/palabras.h> */

#include "basicform.h"
#include "fsm.h"

l_int *insert(l_int *output, int n, char *simb) {
  l_int *aux, *node;

  memtest(node = (l_int *) malloc(sizeof(l_int)));
  node->n    = n;
  node->next = NULL;
  if (n == 0) memtest(node->word = strdup(simb));

  if (output == NULL) return node;
  else {
  	aux = output;
  	while (aux->next != NULL) aux = aux->next;
	aux->next = node;
	return output;
  }
}

l_int *append(l_int *a, l_int *b, int *UNK, char *simb, int flag) {
  l_int *aux, *new = NULL;
  int cont;
  
  aux = a;
  while (aux != NULL) {
	  new = insert(new, aux->n, aux->word);
	  aux = aux->next;
  }

  if (flag == IN) new = insert(new, 0, simb);
  else {
  	aux = b;
  	while (aux != NULL) {
	  if (aux->n != *UNK)
		  new = insert(new, aux->n, NULL);
	  else new = insert(new, 0, simb);
	  aux = aux->next;
	}
  }

  return new;
}

void free_output(l_int *a) {
  l_int *p, *pp;

  for(p = a; p != NULL; ) {
	  pp = p;
	  p = p->next;
	  if (pp->n == 0) free(pp->word);
	  free(pp);
  }

}

int get_data(char *linea, int *origen, int *destino, char *simbolo, double *prob, int *nprobs, char *output) {
  char *next;
  int i;

  if (sscanf( (const char *) linea, "%d %d", origen, destino) != 2) return 0;
  next = strchr(linea, '"');
  next++;

  i = -1;
  while (*next != ' ') {
	  i++;
	  simbolo[i] = *next;
	  next++;
  }
  simbolo[i] = '\0';
  next++;

  i = 0;
  while (sscanf( (const char *) next, "p = %lf", &prob[i]) > 0) {
  	next = strchr(next, ' '); next++;
  	next = strchr(next, ' '); next++;
  	next = strchr(next, ' '); next++;
	i++;
  }
  *nprobs = i;
  /* sscanf( (const char *) next, "p = %lf", &prob[0]);
  next = strchr(next, '"');
  next++;
  *nprobs = 1; */

  next = strchr(next, '"');
  next++;
  for (i = 0; *(next+1) != '\n'; i++, next++) output[i] = *next;
  output[i] = '\0';

  return 1;

}

void get_input_output(T_REDGEN *A, char *symbols, int *input_id, l_int **output_ids) {
  char *next, *ending;
  int length_in, length_out;
  char input[MAXLINE], output[MAXLINE], aux[MAXLINE];
  int *index;
  l_int *node, *new, *prev;

  *output_ids = NULL;

  next = strchr(symbols, ' ');
  if (next != NULL) {
  	length_in = next - symbols;
  	strncpy(input, symbols, length_in);
  	input[length_in] = '\0';

	strcpy(aux, next+1);
	next = aux;
	while (next != '\0') {
		ending = strchr(next, ' ');
		if (ending != NULL) length_out = ending - next;
		else length_out = strlen(next);
		strncpy(output, next, length_out);
		output[length_out] = '\0';

  		index = hs(output, length_out+1, A->table_out);
  		if (index == (int *) -1) {
			memtest(index = (int *) malloc(sizeof(int)));
			*index = A->n_out;
       			hi(output, length_out+1, index, A->table_out);
			A->n_out++;
  		}
		memtest(new = (l_int *) malloc(sizeof(l_int)));
		new->n    = *index;
		new->next = NULL;

		prev = NULL;
		node = *output_ids;
		while (node != NULL) {
			prev = node;
			node = node->next;
		}
		if (prev == NULL) *output_ids = new;
		else prev->next = new;

		if (ending != NULL) next = ending + 1;
		else break;
	}
  }
  else {
	  length_in = strlen(symbols);
	  strcpy(input, symbols);
  }

  /* Input symbol */

  index = hs(input, length_in+1, A->table_in);
  if (index == (int *) -1) {
	memtest(index = (int *) malloc(sizeof(int)));
	*index = A->n_in;
       	hi(input, length_in+1, index, A->table_in);
	A->n_in++;
  }
  *input_id = *index;

}

T_REDGEN *CrearRed(int ne, int hts) {
  T_REDGEN  *A;
  int i;

  memtest(A = (T_REDGEN *) malloc(sizeof(T_REDGEN)));
  A->ne = ne;
  memtest(A->est = (T_ESTADO *) malloc(ne*sizeof(T_ESTADO)));

  for (i = 0; i < ne; i++) {
  	A->est[i].initial = A->est[i].final = INT_MAX;
  	A->est[i].cabeza = A->est[i].cola = A->est[i].BACKOFF = A->est[i].UNKNOWN = NULL;
  }

  A->table = hc(hts);

  A->table_in = hc(hts);
  A->n_in = 1;

  A->table_out = hc(hts);
  A->n_out = 1;

  return A;
}

void enter_state(T_REDGEN *A, int state, double ini, double fin) {

  A->est[state].initial = ini;
  A->est[state].final = fin;
  A->est[state].cabeza = A->est[state].cola = A->est[state].BACKOFF = A->est[state].UNKNOWN = NULL;
	  
  if (ini < INT_MAX) A->ini = state;

  A->est[state].active = 1;
}

void enter_transition(T_REDGEN *A, int stage, int type, int source, int target, char *symbols, double weight, lexicon *l) {
  T_ARISTA *arista, *aux, *arista2;
  int input, i;
  char key[MAXLINE];
  l_int *output = NULL;

  get_input_output(A, symbols, &input, &output);

  //fprintf(stderr, "%d ", i);
  //for (i = 1; i <= l->n; i++) fprintf(stderr, "%f ", l->p[i]);
  //fprintf(stderr, "\n");

  memtest(arista     = (T_ARISTA *) malloc(sizeof(T_ARISTA)));
  arista->origen     = source;
  arista->dest       = target;
  arista->input      = input;
  arista->output     = output;
  arista->prob[0]    = weight;
  arista->np	     = 1;
  //if ((weight > 0) && (l != NULL)) {
  if (l != NULL) {
	for (i = 1; i <= l->n; i++) arista->prob[i] = l->p[i];
	arista->np  += l->n;
  }
  arista->siguiente = NULL;

  switch (type) {
	  case Backoff: A->est[source].BACKOFF = arista;
			break;
	  case Unknown: A->est[source].UNKNOWN = arista;
			break;
			
	  case Transit: memtest(arista2    = (T_ARISTA *) malloc(sizeof(T_ARISTA)));
  			arista2->origen  = source;
  			arista2->dest    = target;
  			arista2->input   = input;
  			arista2->output  = output;
  			arista2->prob[0] = weight;
  			arista2->np      = 1;
  			//if ((weight > 0) && (l != NULL)) {
  			if (l != NULL) {
				for (i = 1; i <= l->n; i++) arista2->prob[i] = l->p[i];
				arista2->np += l->n;
  			}
  			arista2->siguiente = NULL;

			switch (stage) {
				case Training: if (A->est[source].cabeza == NULL)
						       A->est[source].cabeza = arista;
					       else
						       A->est[source].cola->siguiente = arista;
					       A->est[source].cola = arista;

					       //break;

				case Decoding: sprintf(key, "%d %d", source, input);
					       aux = hs(key, strlen(key)+1, A->table);
					       if (aux == (T_ARISTA *) -1)
						   hi(key, strlen(key)+1, arista2, A->table);
					       else {
						   while (aux->siguiente != NULL) aux = aux->siguiente;
						   aux->siguiente = arista2;
					       }
					       break;
			}
			break;
  }
}

void get_final_probs(T_REDGEN *A) {
  int i, e;
  double p;

  for (i = 1; i < A->ne ; i++) {
	  if (A->est[i].final == INT_MAX) {
	  	e = i;
	  	p = 0;
	  	do {
		  if (A->est[e].BACKOFF != NULL) {
		  	p = p + A->est[e].BACKOFF->prob[0];
		  	e = A->est[e].BACKOFF->dest;
		  }
		  else break;
	  	} while (A->est[e].final == INT_MAX);
	  	p = p + A->est[e].final;
	  	A->est[i].final = p;
	  }
  }
}

void get_vocabulary(T_REDGEN *A, int flag) {
  int i, end;
  h_t *table;
  he_t *p;
  char **vocabulary;
  
  if (flag == IN) {
	  end   = A->n_in;
	  table = A->table_in;
  }
  else {
	  end   = A->n_out;
	  table = A->table_out;
  }

  vocabulary = (char **) malloc(end * sizeof(char *));
  for(i = 0; i < table->hsize; i++)
	for (p = table->htable[i]; p != NULL; p = p->p)
	  	vocabulary[*((int *) p->data)] = strdup(p->key);

  if (flag == IN) A->alfa_in = vocabulary;
  else A->alfa_out = vocabulary;

}

T_REDGEN *read_fsm(char *file, int hts) {
  FILE *fd_input;
  char linea[MAXLIN], output[MAXLIN], simbolo[MAXLIN];
  char nom[MAXLINLR], ident[MAXLINLR];
  int origen, destino;
  double checksum, initial, final, prob[MaxProb], p;
  int i, type, ne, nprobs;
  T_REDGEN *A;
  lexicon lex, *l = NULL;

  if ((fd_input = fopen(file, "r")) == NULL ) {
  	fprintf(stderr, "Failed open: transducer file <%s>\n", file);
	exit(EXIT_FAILURE);
  }


  do fgets(linea, MAXLIN, fd_input);
  while (LeerNombre(linea, nom) != SI_NOMBRE);

  do fgets(linea, MAXLIN, fd_input);
  while (LeerNumEstados(linea, &ne) <= 0);

  A = CrearRed(ne, hts);
  strcpy(A->nombre, nom);

  while (fgets(linea, MAXLIN, fd_input) != NULL) {
   	if (LeerEstado(linea, ident, &checksum, &initial, &final, output) > 0) {
		//if (atoi(ident) % 1000000 == 0) fprintf(stderr, "--> Leyendo estado %s\n", ident);
    		if (initial > 0) initial = -log10(initial);
    		else initial = INT_MAX;
    		if (final > 0) final = -log10(final);
    		else final = INT_MAX;
    		enter_state(A, atoi(ident), initial, final);
   	}
   	else if (get_data(linea, &origen, &destino, simbolo, prob, &nprobs, output)) {
		/* LeerArista(linea, origen, destino, simbolo, &prob, output) > 0) */
		/* 0 1 "#" p = 0.142857 o = "# "" */

    		if (!strcmp(simbolo, "<BACKOFF>")) type = Backoff;
    		else if (!strcmp(simbolo, "<unk>")) type = Unknown;
    		else type = Transit;

    		if (*output != '\0') {
			strcat(simbolo, " ");
    			strcat(simbolo, output);
		}

		/* if (prob[0] > 0) p = -log10(prob[0]);
		else p = INT_MAX; */
		lex.n = nprobs - 1;
		if (nprobs > 1) {
			for (i = 1; i < nprobs; i++)
				if (prob[i] > 0) lex.p[i] = -log10(prob[i]);
				else lex.p[i] = INT_MAX;
			l = &lex;
		}
		else l = NULL;
    		enter_transition(A, Decoding, type, origen, destino, simbolo, -log10(prob[0]), l);
    		//enter_transition(A, Training, type, origen, destino, simbolo, -log10(prob[0]), l);
   	} 
  }

  fclose(fd_input);

  get_vocabulary(A, IN);
  get_vocabulary(A, OUT);

  return A;
  
}

void get_output(char **source, char **dt, l_int *output, char *candidate) {
  l_int *aux;

  *candidate = '\0';
  aux = output;
  while (aux != NULL) {
	//fprintf(stderr, "%d ", aux->n); fflush(stderr);
	if (aux->n > 0) strcat(candidate, dt[aux->n]);               // Output from parsing
	else if (aux->n < 0) strcat(candidate, source[-(aux->n)]);   // Conversión lema-palabra
	else strcat(candidate, aux->word);                           // Palabras desconocidas
	aux = aux->next;
	if (aux != NULL) strcat(candidate, " ");
  }
  //fprintf(stderr, "\n");fflush(stderr);
}

void write_output(char **dictionary, l_int *output, FILE *fd_output) {
  l_int *aux;

  aux = output;
  while (aux != NULL) {
	fprintf(fd_output, "%s", dictionary[aux->n]);
	aux = aux->next;
	if (aux != NULL) fprintf(fd_output, " ");
  }
  fprintf(fd_output, "\n");
}


void write_fsm(T_REDGEN  *A, char *file, int n, struct options *opt) {

  int i, j;
  double     z;
  T_ARISTA   *arista;
  FILE *fd_output;
  l_int *aux;
  char candidate[MAXLINE];

  if ((fd_output = fopen(file, "w")) == NULL ) {
  	fprintf(stderr, "Failed write: transducer file <%s>\n", file);
	exit(EXIT_FAILURE);
  }


  fprintf(fd_output, "Name %s\n\n", A->nombre);
  fprintf(fd_output, "NumStates %d\n\n", A->ne);

  for (i = 0; i < A->ne; i++) {
   if (A->est[i].initial < INT_MAX) z = pow(10, -A->est[i].initial); 
   else z = 0;
   fprintf(fd_output, "\nState %d i = %lf f = %g\n", i, z, pow(10, -opt->l[0]*A->est[i].final));

   arista = A->est[i].cabeza;
   while (arista != NULL) {
     get_output(A->alfa_in, A->alfa_out, arista->output, candidate);
     fprintf(fd_output, "%d %d \"%s\" ", arista->origen, arista->dest, A->alfa_in[arista->input]);
     z = 0;
     if ((n == 1) && ((opt->l[1] != 0) || (opt->l[2] != 0) || (opt->l[3] != 0) || (opt->l[4] != 0) || (opt->l[5] != 0)))
	for (j = 1; j < arista->np; j++)
		z += opt->l[j] * arista->prob[j];
     //fprintf(fd_output, "kk = %g ", z);
     for (j = 0; j < n && j < arista->np; j++, z = 0) {
     	z += opt->l[j] * arista->prob[j];
	fprintf(fd_output, "p = %g ", pow(10, -z));
     }
     fprintf(fd_output, "o = \"%s\"\n", candidate);
     /* fprintf(fd_output, "%d %d \"%s\" p = %g o = \"%s\"\n", arista->origen, arista->dest, A->alfa_in[arista->input],
   		    				          pow(10, -arista->prob[0]), candidate); */

     arista = arista -> siguiente;
   } 
   arista = A->est[i].BACKOFF;
   if (arista != NULL) fprintf(fd_output, "%d %d \"%s\" p = %g o = \"\"\n", arista->origen, arista->dest,
		               A->alfa_in[arista->input], pow(10, -opt->l[0]*arista->prob[0]));
   arista = A->est[i].UNKNOWN;
   if (arista != NULL) {
	   get_output(A->alfa_in, A->alfa_out, arista->output, candidate);
	   fprintf(fd_output, "%d %d \"%s\" p = %g o = \"%s\"\n", arista->origen, arista->dest,
		   	       A->alfa_in[arista->input], pow(10, -opt->l[0]*arista->prob[0]), candidate);
   }

  }

  fclose(fd_output);
}

void free_fsm(T_REDGEN *A) {
  int i;
  T_ARISTA *arista, *aux;
  l_int *p, *pp;
  he_t *a, *aa;
  time_t clock;

  for (i = 0; i < A->ne; i++) {
   arista = A->est[i].cabeza;
   while (arista != NULL) {
    aux = arista;
    arista = arista -> siguiente;
    free_output(aux->output);
    //for(p = aux->output; p != NULL; pp = p, p = p->next, free(pp), pp=NULL);
    free(aux);
   }
   if (A->est[i].BACKOFF != NULL) {
	   //free(A->est[i].BACKOFF->output);
	   free(A->est[i].BACKOFF);
   }
   if (A->est[i].UNKNOWN != NULL) {
	   //free(A->est[i].UNKNOWN->output);
    	   for(p = A->est[i].UNKNOWN->output; p != NULL; pp = p, p = p->next, free(pp));
	   free(A->est[i].UNKNOWN);
   }
  }
  free(A->est);

  for(i = 0; i < A->table->hsize; i++) {
	  //fprintf(stderr, "%d\n", i); fflush(stderr);
	  for (a = A->table->htable[i]; a != NULL; aa = a, a = a->p, free(aa), aa = NULL) {
		free(a->key);
		a->key = NULL;
		arista = a->data;
  	  	while (arista != NULL) {
		  aux = arista;
		  arista = arista->siguiente;
		  //free_output(aux->output);
		  free(aux);
	  	}
	  }
  }
  free(A->table->htable);
  free(A->table);
  //hd(A->table, 1);

  clock = time(NULL);
  //fprintf(stderr, "FIN LIBERACION MEMORIA TABLA DE ARISTAS... %s", asctime(localtime(&clock))); fflush(stderr);

  hd(A->table_in, 1);
  hd(A->table_out, 1);
  
  for (i = 1; i < A->n_in; i++) free(A->alfa_in[i]);
  free(A->alfa_in);

  for (i = 1; i < A->n_out; i++) free(A->alfa_out[i]);
  free (A->alfa_out);

  free(A);
}
