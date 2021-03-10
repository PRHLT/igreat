#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "fsm.h"
#include "refx.h"
#include "l2w.h"
#include "parse.h"

ea **reserva_memoria_trellis(int size) {
  ea **lea;
  
  memtest(lea = (ea **) malloc(3*sizeof(ea *)));
  memtest(lea[0] = (ea *) malloc(size*sizeof(ea)));
  memtest(lea[1] = (ea *) malloc(size*sizeof(ea)));
  memtest(lea[2] = (ea *) malloc(size*sizeof(ea)));
  
  return lea;
}

void anyade_estados_iniciales(T_REDGEN *A, ea **lea, int *contea, T_REDGEN *B) {
  
  lea[0][0].estado = A->ini;
  lea[0][0].score = 0;
  lea[0][0].input = 0;
  //lea[0][0].BO = 0;
  lea[0][0].output = NULL;
  //lea[0][0].origen = A->ini;
  if (B != NULL) lea[0][0].lm_estado = B->ini;
  (*contea)++;

  /* lea[0][*contea+i].from = wc;
  lea[0][*contea+i].siguiente = -1;
  strcpy(lea[0][*contea+i].hipotesis,"");
  strcpy(lea[0][*contea+i].input,""); */
}

void escribe_lea(ea **lea, int which, int num, char **dictionary) {
  int i;
  char candidate[MAXLINE];
  l_int *aux;
  
  fprintf(stderr, "\nLista de Estados Activos (%d)\n", num);
  for (i = 0; i < num; i++) {
  	fprintf(stderr, "%d + (%d lm) (Score = %g) ::: Input = %d ::: Output = ",
		lea[which][i].estado, lea[which][i].lm_estado, lea[which][i].score, lea[which][i].input);

	aux = lea[which][i].output;
	while (aux != NULL) {
		fprintf(stderr, "%d ", aux->n);
		aux = aux->next;
	}
	fprintf(stderr, "\n");
	//get_output(vocab, dictionary, lea[which][i].output, candidate);
	//printf("%s\n", candidate);
	//write_output(dictionary, lea[which][i].output, stderr);
  }
  fprintf(stderr, "\n");
  fflush(stderr);
}

int inserta_ea(ea *lea, int start, int *contea, l_int *previous, T_ARISTA *a, double score, int *UNK, int n,
//int BO,
char *simb, int type, struct options *opt, char **tags, char **dict, h_t *table, h_t *vocab, int lm) {
  int j, k, l;
  int found = 0;
  int flagUNK;
  l_int input, *tmp;
  ea aux;

  for (j = start; j < *contea && lea[j].score <= score; j++) {
	if ((lea[j].estado == a->dest) && (lea[j].input == n)) { /* PELIGRO :: REVISAR ESTA LINEA */
		found = 1;					 /* QUIZA :: && BO >= lea[j].BO   */
		break;						 /* BORRAR pos J, INSERTAR en nueva POS */
	}
  }

  //fprintf(stderr, "Le tocaría situarse en la posicion %d\n", j);

  if (!found) {
	  for (k = j; k < *contea; k++)
		  if ((lea[k].estado == a->dest) && (lea[k].input == n)) {
			//if (BO > lea[k].BO) found = 2;
			//else found = 1;
			found = 1;
		  	break;
		  }

	  if (opt->algorithm == 0 || found < 2) {

	  	if (found) free_output(lea[k].output);
  	  	//fprintf(stderr, "Hay que correr desde %d..%d\n", j, k);

	  	for (l = k; l > j; l--) {
		  if (l < opt->beam)
			  lea[l] = lea[l - 1];
		  else free_output(lea[l-1].output);
		  //printf("Escribiendo %d sobre %d: \"", l-1, l);
	  	  //write_output(dictionary, lea[l].output, stdout); 
	  	}

	  	if (j < opt->beam) {
	  		lea[j].estado    = a->dest;
	  		lea[j].score     = score;
	  		lea[j].input     = n;
	  		lea[j].lm_estado = lm;
	  		//lea[j].BO     = BO;
			//lea[j].origen = a->dest;

			if ((opt->reord) && (type != Backoff)) {

				/* Procesar a->output = lema numero lema numero lema numero ... 
			 	*
			 	* Coger cada lema, encontrar su tags[numero]
			 	* consultar la tabla y emitir la salida */

				//fprintf(stderr, "Palabra %s\n", simb);
				//write_output(dict, a->output, stdout);

				tmp = read_entry(a->output, dict, tags, n, table, vocab);
	  			lea[j].output = append(previous, tmp, UNK, simb, OUT);

				//printf("Simbolo %d: %s %s\n", *simb, words[*simb], tags[*simb]);
				/* input.n = a->input;
				input.next = NULL;
				tmp = append(previous, &input, UNK, simb, IN);
	  			lea[j].output = append(tmp, a->output, UNK, simb, OUT); */

	  			free_output(tmp);
			}
			else lea[j].output = append(previous, a->output, UNK, simb, OUT);

	  		if ((!found) && (*contea < opt->beam)) (*contea)++;
	  	}
	  }

	  	/* printf("Escribiendo sobre %d: Concatenar1 = \"", j);
	  	write_output(dictionary, previous, stdout); 
	  	printf("Escribiendo sobre %d: Concatenar2 = \"", j);
	  	write_output(dictionary, a->output, stdout); */

	  	//printf("Escribiendo sobre %d: Resul = \"", j);
	  	//write_output(dictionary, lea[j].output, stdout); 
	  
  }
  return j;

	  /* Insertar en posición j
	      // Corriendo de j..contea un lugar a la derecha (max beam)
	      // Si encontrásemos el estado a->dest desaparece */

				/* Busca si a->dest está ya en lea
				 * si SI entonces ver si score < lea[j].score
				 * 	=> sustituir el actual por el que había
				 * si NO entonces insertar el actual */
}

void actualiza_trellis(ea **lea, int source, int cont, int target, int contea) {
  int j;
  l_int *p, *pp;

  for (j = 0; j < cont; j++) {
	  free_output(lea[source][j].output);
	  lea[source][j].output = NULL;
  }
  for (j = 0; j < contea; j++)
	  lea[source][j] = lea[target][j];
}


void free_trellis(ea **lea) {
	free(lea[0]);
	free(lea[1]);
	free(lea[2]);
	free(lea);
}

l_int *addlista(int state, l_int *lista) {
  l_int *new, *aux;

  new = (l_int *) malloc(sizeof(l_int));
  new->n = state;
  new->word = NULL;
  new->next = lista;

  return new;
}

/* void updatelista(T_REDGEN *A, l_int *lista, int donothing) {
  l_int *aux;

  if (!donothing) {
	aux = lista;
	while (aux != NULL) {
		A->est[aux->n].active = 1;
		aux->n = A->est[aux->n].BACKOFF->dest;
		A->est[aux->n].active = 0;
		aux = aux->next;
	}
  }
} */

void freelista(T_REDGEN *A, l_int *lista) {
  l_int *aux, *before;

  aux = lista;
  while (aux != NULL) {
	A->est[aux->n].active = 1;
	before = aux;
	aux = aux->next;
	free(before);
  }
}

void printlista(l_int *lista) {
  l_int *aux;

  fprintf(stderr, "------------\n");
  aux = lista;
  while (aux != NULL) {
	fprintf(stderr, "ESTADO %d\n", aux->n);
	aux = aux->next;
  }
  fprintf(stderr, "------------\n");
}

double normalise(T_REDGEN *A, char *key, h_t *table, l_int *lista, int save) {
  double *pt, norm, acum, res;
  int estado, simbolo, found;
  char key2[MAXLINE], key3[MAXLINE], *next, *start, *stop;
  T_ARISTA *a, *aa;

  //fprintf(stderr, "Normalizando para el estado+input %s\n", key);
  pt = hs(key, strlen(key)+1, table);
  if ((save) && (pt != (double *) -1)) norm = *pt;
	//fprintf(stderr, "Encontrada ::: ");
  else {
	acum = 0.0;

	sscanf( (const char *) key, "%d %d", &estado, &simbolo);
	start = strchr(key, ' ');
	start++;
	stop = start;
	start = strchr(start, ' ');
	if (start != NULL) start++; 

	sprintf(key2, "%d %d", estado, simbolo);
	a = hs(key2, strlen(key2)+1, A->table);
	if (a != (T_ARISTA *) -1) {
		for (next = start; a != NULL; a = a->siguiente, next = start) {
			aa = a;
			//found = 0;
			while ((a != (T_ARISTA *) -1) && (A->est[a->dest].final == INT_MAX)
			    && (next != NULL)) {
				sscanf( (const char *) next, "%d", &simbolo);
		           	sprintf(key2, "%d %d", a->dest, simbolo);
			   	a = hs(key2, strlen(key2)+1, A->table);

			   	next = strchr(next, ' ');
				if (next != NULL) next++;
			}
		        if ((a != (T_ARISTA *) -1) && (A->est[a->dest].final < INT_MAX) &&
			    (next == NULL) && (A->est[a->dest].active)) {
				acum = acum + pow(10, -aa->prob[0]);
				lista = addlista(a->dest, lista);
				A->est[a->dest].active = 0;
			}
			a = aa;
		}
	}

        //printlista(lista);
        //printlista(new);
	
	if ((a = A->est[estado].BACKOFF) != NULL) {
		//updatelista(A, lista, save);
		
		sprintf(key3, "%d %s", a->dest, stop);
		res = normalise(A, key3, table, lista, 0);
  		//fprintf(stderr, "Intermedio ::: %lf ::: %lf ::: %lf\n", acum, pow(10, -a->prob), res);
		//acum = acum + pow(10, -a->prob - res);
		acum = acum + pow(10, -a->prob[0])*res;
	}
	else freelista(A, lista);

	/* Acumular el peso de backoff * resultado mismo proceso sobre destino */

	/* if (acum != 0.0) norm = -log10(acum);
	else norm = INT_MAX; */
	norm = acum;

	if (save) {
		memtest(pt = (double *) malloc(sizeof(double)));
		*pt = norm;
        	hi(key, strlen(key)+1, pt, table);
	}
	//fprintf(stderr, "Calculada ::: ");
  }
  //fprintf(stderr, "Key ::: %s ::: %lf\n", key, norm);
  return norm;
}

double miniparselm(l_int *output, T_REDGEN *A, T_REDGEN *B, int *lm_estado, char *simb, int *UNK) {
  l_int *aux = output;
  char key[MAXLINE];
  int *index;
  T_ARISTA *arista, *arco;
  double res = 0;

  //fprintf(stderr, "aux->n = %d <> %d\n", aux->n, *UNK); fflush(stderr);
  while (aux != NULL) {
	if (aux->n == *UNK)       index = hs(simb,                  strlen(simb)+1,                  B->table_in);
	else if (aux->n > 0)      index = hs(A->alfa_out[aux->n]  , strlen(A->alfa_out[aux->n])+1  , B->table_in);
	else                      index = hs(A->alfa_in[-(aux->n)], strlen(A->alfa_in[-(aux->n)])+1, B->table_in);

	do {
		arista = (T_ARISTA *) -1;
		if (index != (int *) -1) {
			sprintf(key, "%d %d", *lm_estado, *index);
			arista = hs(key, strlen(key)+1, B->table);
		}

		if ((index != (int *) -1) && (arista != (T_ARISTA *) -1)) arco = arista;
		else if (B->est[*lm_estado].BACKOFF != NULL)              arco = B->est[*lm_estado].BACKOFF;
		else if ((arista = B->est[*lm_estado].UNKNOWN) != NULL)   arco = arista;

		*lm_estado = arco->dest;
		res = res + arco->prob[0];
  		//fprintf(stderr, "LM score = %f\n", res);
	} while (arista == (T_ARISTA *) -1);

	aux = aux->next;
  }
  return res;
}

double newscore(double oldscore, T_ARISTA *a, struct options *opt,
		T_REDGEN *A, T_REDGEN *B, int *lm_estado, char *simb, int *UNK) {
  double score = 0;
  int i;

  //fprintf(stderr, "LM state = %d\n", *lm_estado);

  for (i = 0; i < opt->ngram && i < a->np; i++)
	if ((opt->l[i] != 0) && (a->prob[i] == INT_MAX)) return -1;
	else score = score + opt->l[i]*a->prob[i]; 

  //fprintf(stderr, "TR score = %f\n", score);

  if (B != NULL)
	if (a->output != NULL) score = score + opt->norm*miniparselm(a->output, A, B, lm_estado, simb, UNK);
	else                   score = score + opt->norm*B->est[*lm_estado].final;

  //fprintf(stderr, "TR+LM score = %f\n", score);

  score = oldscore + score;

  return score;
  
}

h_t *explore(T_REDGEN *A, int state, int **index, int i, int n, double oldscore, l_int *lista, int save,
	    //int BO,
	    ea **lea, int *contea, l_int *previous, int *UNK, char *simb, struct options *opt,
	    char **tags, h_t *table, h_t *vocab, int *contaux, int lm_estado, int *found, T_REDGEN *B) {
  char key[MAXLINE], key2[MAXLINE], key3[MAXLINE];
  T_ARISTA *a, *aa;
  int j = i, k, z, mini = state;
  double score, *pt;
  h_t *aux = NULL, *below;
  he_t *p,*pp;
  int lm = lm_estado;

  //fprintf(stderr, "Estado %d...\n", state);

  do {
  if (index[j] != (int *) -1) {
	sprintf(key, "%d %d", mini, *(index[j]));
        a = hs(key, strlen(key)+1, A->table);
	if (a != (T_ARISTA *) -1) {
		//*found = 1; // ######### PELIGRO #########
		//fprintf(stderr, "Arista compatible: %d => %d (%g)\n",
		//	a->origen, a->dest, A->est[a->dest].final);
		for (aa = NULL; a != NULL; a = a->siguiente) {
  			lm = lm_estado;
			//aa = a;
		        if (A->est[a->dest].final < INT_MAX) {
		     		score = newscore(oldscore, a, opt, A, B, &lm, simb, UNK);

				if (score >= 0) {
					*found = 1;
			    		if (A->est[a->dest].active) {
						if ((opt->algorithm) && (!((state == 0) && (a->dest == 0)))) {
							z = a->dest;
							while (z != 0) {
								lista = addlista(z, lista);
								A->est[z].active = 0;
								z = A->est[z].BACKOFF->dest;
							}
						}
						//fprintf(stderr, "--->Encontrada: %d => %d\n",
							//a->origen, a->dest); fflush(stderr);
						inserta_ea(lea[2], 0, contaux, previous, a, score, UNK, j,
					   	   	   simb, Transit, opt, tags, A->alfa_out, table, vocab, lm);
					}
				}
			}
			else aa = a;
		     	//score = oldscore + a->prob[0];
		     	//score = newscore(oldscore, a, opt);
		     
		     	/* if (((*contea == 0) && (*contaux == 0)) ||
			     ((*contea == 0) && (score < 3.5*lea[2][0].score)) ||
		     	     ((*contea >  0) && (score < 3.5*lea[1][0].score))) { // Factor de DOUBLE BEAM */
		     	//if (score >= 0) {
		}
		j++;
		if (aa != NULL) mini = aa->dest;
	}
  }
  } while ((a != (T_ARISTA *) -1) && (aa != NULL) && (j < n) && (index[j] != (int *) -1));

  //printlista(lista);
  if (opt->algorithm || !(*found)) {
  	if ((a = A->est[state].BACKOFF) != NULL) {
		fprintf(stderr, "Explorando BACKOFF\n");
		//updatelista(A, lista, save);
		//score = oldscore + a->prob[0];
		score = newscore(oldscore, a, opt, NULL, NULL, &lm_estado, NULL, UNK);

		below = explore(A, a->dest, index, i, n, score, lista, 0,
		              lea, contea, previous, UNK, simb, opt, tags, table, vocab, contaux, lm_estado, found, B);
		/* if (opt->norm) {
			//fprintf(stderr, "Calculando BACKOFF (%lf) * Sigma\n", pow(10, -a->prob)); fflush(stderr);
			for(k=0;k<below->hsize;k++)
    				for (p=below->htable[k];p!=NULL;pp=p,p=p->p,free(pp),pp=NULL) {
			   		pt = hs(p->key, strlen(p->key)+1, aux);
					if (pt != (double *) -1) *pt = *pt + pow(10, -a->prob[0]) * *((double *) p->data);
					else {
						memtest(pt = (double *) malloc(sizeof(double)));
				                *pt = pow(10, -a->prob[0]) * *((double *) p->data);
                				hi(p->key, strlen(p->key)+1, pt, aux);
						//fprintf(stderr, "done ... Sigma = %g\n", *pt); fflush(stderr);
					}

					free(p->key);
					p->key=NULL;
					free(p->data);
					p->data=NULL;
				}
				free(below->htable);below->htable=NULL;
				free(below);below=NULL;
			//fprintf(stderr, "done ... BACKOFF * Sigma = %g\n", *pt); fflush(stderr);

		} */
  	}
  	else {
		if (((a = A->est[state].UNKNOWN) != NULL) && (!(*found))) {
			//score = oldscore + a->prob[0];
			//fprintf(stderr, "Palabra desconocida en el TR\n"); fflush(stderr);
			score = newscore(oldscore, a, opt, NULL, B, &lm, simb, UNK);
			inserta_ea(lea[2], 0, contaux, previous, a, score, UNK, i, //BO+1,
				   simb, Unknown, opt, tags, A->alfa_out, table, vocab, lm);
		}
  		freelista(A, lista);
  	}
  }
  return aux;
}

l_int *parse(T_REDGEN *A, int *UNK, char **source, char *corpus, int n, ea **lea, struct options *opt, char *eagles, h_t *table, h_t *vocab, T_REDGEN *B) {
  char simb[MAXLINE], key[MAXLINE], key2[MAXLINE], key3[MAXLINE], *next, *next2, *next3;
  char *tags[MAXLINE];
  int **index;
  int k, cont, contea, contaux;
  int e, i, j, l;
  T_ARISTA *a, *aa;
  double score, *pt;
  int found;
  l_int *output;
  he_t *p;

  memtest(index = (int **) malloc(n*sizeof(int *)));
  for (i = 1; i < n; i++) index[i] = hs(source[i], strlen(source[i])+1, A->table_in);

  cont = contea = 0;
  anyade_estados_iniciales(A, lea, &cont, B);

  next2 = eagles; next3 = corpus;
  for (i = 1; i < n; i++) {
  	//escribe_lea(lea, 0, cont, A->alfa_out);

	strcpy(simb, source[i]);

	//fprintf(stderr, "Parseando símbolo %d/%d (%s)...\n", i, n-1, source[i]); fflush(stderr);

	if (opt->reord == 1) {
		sscanf( (const char *) next2, "%s", simb);
		tags[i] = strdup(simb);
		sscanf( (const char *) next3, "%s", simb);
	}


	for (k = 0; k < cont; k++) {
	   if (lea[0][k].input < i) {
	      e = lea[0][k].estado;

	      contaux = found = 0;
	      explore(A, e, index, i, n, lea[0][k].score, NULL, 1,
		      	    //lea[0][k].BO,
		      	    lea, &contea, lea[0][k].output, UNK, simb, opt, tags, table, vocab,
			    &contaux, lea[0][k].lm_estado, &found, B);
	      //found = 1;
/* ---------------------------- PELIGROOOOOOOOOOOOOOOOOOO ---------------------------- */
  		memtest(aa = (T_ARISTA *) malloc(sizeof(T_ARISTA)));
		for (j = 0; j < contaux; j++) {
			aa->dest = lea[2][j].estado;
			aa->output = NULL;
			score = lea[2][j].score;
			/* if (opt->norm) {
  				if (index[i] != (int *) -1) sprintf(key, "%d %d", lea[0][k].origen, *(index[i]));
				else sprintf(key, "%d -1", lea[0][k].origen);
				pt = hs(key, strlen(key)+1, tableNORM);
				score = score + log10(*pt);
  				//fprintf(stderr, "%lf\n", log10(*pt)); fflush(stderr);
			} */
			inserta_ea(lea[1], 0, &contea, lea[2][j].output, aa, score, UNK, lea[2][j].input,
				   //lea[2][j].BO,
			   	   simb, Transit, opt, tags, A->alfa_out, table, vocab, lea[2][j].lm_estado);
	  		free_output(lea[2][j].output);
	  		lea[2][j].output = NULL;
		}
		free(aa);
/* ---------------------------- PELIGROOOOOOOOOOOOOOOOOOO ---------------------------- */

	      /* //fprintf(stderr, "Estado %d...\n", e);
	      found = 0;
	      if (index[i] != (int *) -1) {
	        sprintf(key, "%d %d", e, *(index[i]));
	        a = hs(key, strlen(key)+1, A->table);
	        if (a != (T_ARISTA *) -1) {
		  //found = 1; ######### PELIGRO #########
		  for (sprintf(key3, "%d %d", lea[0][k].origen, *(index[i]));
		       a != NULL; a = a->siguiente,
		       sprintf(key3, "%d %d", lea[0][k].origen, *(index[i]))) {
		     aa = a;
		     score = lea[0][k].score + a->prob;

		     //if ((contea == 0) || (score < 3.5*lea[1][0].score)) { // Factor de DOUBLE BEAM

		        j = i + 1;
		        //fprintf(stderr, "Arista compatible: %d => %d (%g)\n",
				     //a->origen, a->dest, A->est[a->dest].final);
				     
		        // ######### PELIGRO #########
		     
		        while ((a != (T_ARISTA *) -1) && (A->est[a->dest].final == INT_MAX)
			    && (j < n) && (index[j] != (int *) -1)) {
		           sprintf(key2, "%d %d", a->dest, *(index[j]));
			   sprintf(key3, "%s %d", key3, *(index[j]));
			   //fprintf(stderr, "->Probando: %s(%d %s)\n", key2, a->dest, source[j]);
			   a = hs(key2, strlen(key2)+1, A->table);
			   if (a != (T_ARISTA *) -1)
				//fprintf(stderr, "-->Avanzamos: %d => %d\n", a->origen, a->dest);
			   j++;
		        }

		        if ((a != (T_ARISTA *) -1) && (A->est[a->dest].final < INT_MAX)) {
	          	   found = 1;

		    	   //printf("Previous => \"");
		    	   //write_output(A->alfa_out, lea[0][k].output, stdout); 

			   //fprintf(stderr, "--->Encontrada: %d => %d\n", a->origen, a->dest); fflush(stderr);
			   if (opt->norm) score = score + log10(normalise(A, key3, tableNORM, NULL, 1));

		    	   inserta_ea(lea[1], 0, &contea, lea[0][k].output, a, score, UNK, j-1, //lea[0][k].BO,
				      simb, Transit, opt, tags, A->alfa_out, table, vocab);
			
	  	    	   //printf("Next => \"");
		    	   //write_output(A->alfa_out, lea[1][kk].output, stdout); 

			    * Busca si a->dest está ya en lea[1]
			    * si SI entonces ver si score < lea[1][j].score
			    * 	=> sustituir el actual por el que había
			    * si NO entonces insertar el actual *
		        }
		        a = aa; // ######### PELIGRO #########
		     //}
	          }
	        }
	      }
	      //if ((opt->algorithm || !found) && ((a = A->est[e].UNKNOWN) != NULL)) {
	      if ((!found) && ((a = A->est[e].UNKNOWN) != NULL)) {
		// CAMBIANDO POR LO DEL DICCIONARIO
		//fprintf(stderr, "Explorando UNKNOWN\n");
		score = lea[0][k].score + a->prob;
		inserta_ea(lea[1], 0, &contea, lea[0][k].output, a, score, UNK, i, //lea[0][k].BO + 1,
			   simb, Unknown, opt, tags, A->alfa_out, table, vocab);
			// CAMBIANDO POR LO DEL DICCIONARIO
		// if (lea[0][k].estado == 0) {
		// lea[0][k].estado = a->dest;
		// lea[0][k].score += a->prob;
		// lea[0][k].BO++;
		// k--;
		// }
		// else {
		// score = lea[0][k].score + a->prob;
		// inserta_ea(lea[1], 0, &contea, lea[0][k].output, a, score, UNK, i, //lea[0][k].BO + 1,
		// simb, Unknown, opt, tags, A->alfa_out, table, vocab);
		// }
	      }
	      else if ((opt->algorithm || !found) && ((a = A->est[e].BACKOFF) != NULL)) {
		//fprintf(stderr, "Explorando BACKOFF\n");

		lea[0][k].estado = a->dest;
		lea[0][k].score += a->prob;
		//lea[0][k].BO++;
		k--;
		//escribe_lea(lea, 0 , cont, A->alfa_out);
	      } */
	      //else k++;
	   }
	   else {
		//fprintf(stderr, "--->Se mantiene en el trellis: %d\n",lea[0][k].estado);
  		memtest(aa = (T_ARISTA *) malloc(sizeof(T_ARISTA)));
		aa->dest = lea[0][k].estado;
		aa->output = NULL;
		inserta_ea(lea[1], 0, &contea, lea[0][k].output, aa, lea[0][k].score, UNK, lea[0][k].input, //lea[0][k].BO,
			   simb, Transit, opt, tags, A->alfa_out, table, vocab, lea[0][k].lm_estado);
		//k++;
		free(aa);
	   }
	}


	actualiza_trellis(lea, 0, cont, 1, contea);
	cont = contea;
	contea = 0;

	/***********************************************************************************
	 * 	Para toda (word/*kkindex)
	 * 		Para todo estado activo € LEA.OLD
	 * 			Para toda arista compatible (Estado + Simbolo)
	 * 				score = score(Estado) + peso(Arista) (en -log10)
	 * 				HAY QUE MINIMIZAR LOS SCORES
	 * 				if (score < SCORE(LEA.NEW, Arista->Destino)
	 * 			Si no hubiera ninguna arista compatible AND existe arista <unk>
	 * 				score = score(Estado) + peso(Arista) (en -log10)
	 * 				HAY QUE MINIMIZAR LOS SCORES
	 * 				if (score < SCORE(LEA.NEW, Arista->Destino)
	 * 			Si no hubiera ninguna arista compatible AND existe arista BO
	 * 				se añade en esta misma etapa (LEA.OLD)
	 * 					el estado destino de la transicion de BO
	 * 						sumandole el peso de la arista
	 * 		HISTORIC.append(LEA.OLD)
	 * 		LEA.OLD <= los 50(beam) mejores de LEA.NEW (los más pequeños)
	 ***********************************************************************************/

	/* next = strchr(next, ' ');
	if (next != NULL) next++; */

	if (opt->reord) {
		next2 = strchr(next2, ' ');
		if (next2 != NULL) next2++;

		next3 = strchr(next3, ' ');
		if (next3 != NULL) next3++;
	}
  }

  //escribe_lea(lea, 0, cont, A->alfa_out);

  memtest(aa = (T_ARISTA *) malloc(sizeof(T_ARISTA)));
  aa->output = NULL;
  aa->np = 1;
  for (k = 0; k < cont; k++) {
	e = lea[0][k].estado;
  	aa->prob[0] = A->est[e].final;
	//score = lea[0][k].score + A->est[e].final;
	score = newscore(lea[0][k].score, aa, opt, NULL, B, &lea[0][k].lm_estado, NULL, UNK);
	aa->dest = e;
	inserta_ea(lea[1], 0, &contea, lea[0][k].output, aa, score, UNK, lea[0][k].input, //lea[0][k].BO,
		   NULL, Backoff, opt, tags, A->alfa_out, table, vocab, lea[0][k].lm_estado);
  } 

  //escribe_lea(lea, 1, contea, A->alfa_out);

  /************************* PELIGRO *************************/
  if (cont > 0) output = append(lea[1][0].output, aa->output, UNK, NULL, OUT);
  else output = NULL;
  /************************* PELIGRO *************************/
  free(aa);

  actualiza_trellis(lea, 0, cont, 1, contea); 
  cont = contea;
  contea = 0;

  /* Para liberar el output final en lea[0] */
  actualiza_trellis(lea, 0, cont, 1, contea); 

  if (opt->reord) for (k = 1; k < n; k++) free(tags[k]);

  free(index);

  return output;
}
