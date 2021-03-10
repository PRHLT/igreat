#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
/* #include "giati.h" */
#include "fsm.h"
#include "arpa.h"

/********* Begin::: FILTRADO DEL MODELO MEDIANTE EL CORPUS DE TEST *********/

h_t *load_test(char *test, int hts, char *separator, int ngram, char **history) {
  char word[MAXLINE], key[MAXLINE];
  FILE *fd_test;
  h_t *table;
  int i, n, k;
  time_t clock;

  //sprintf(test, "%s/%s", corpus_dir, TEST);

  if ((fd_test = fopen(test, "r")) == NULL ) {
	  fprintf(stderr, "Failed open: test file <%s>\n", test);
	  exit(EXIT_FAILURE);
  }

  n = 1;
  if (ngram > 1) strcpy(history[0], "<s>");

  table = hc(hts);
  while (fscanf(fd_test, "%s", word) != EOF) {
	  if (!strcmp(word, separator))
		  strcpy(word, "</s>");
	  else {
		  if (hs(word, strlen(word)+1, table) == (void *) -1) {
	  	  	//printf("%s\n", word);
		  	hi(word, strlen(word)+1, NULL, table);
		  }
	  }

	  if (ngram > 1) {
	     for (k = n-1; k >= 0; k--) {
	  	strcpy(key, history[k]);
	  	for (i = k+1; i < n; i++)
		  sprintf(key, "%s %s", key, history[i]);
	  	strcat(key, " ");
	  	strcat(key, word);

	  	//printf("%s\n", key);

	  	if (hs(key, strlen(key)+1, table) == (void *) -1)
		  hi(key, strlen(key)+1, NULL, table);
	     }
	  }

	  if (!strcmp(word, "</s>")) {
		  n = 1;
		  if (ngram > 1) strcpy(history[0], "<s>");
		  fscanf(fd_test, "%*[^\n]");
		  fscanf(fd_test, "%*1[\n]");
	  }
	  else {
		  if (n >= ngram - 1) {
			  for (i = 0 ; i < ngram-2; i++)
				  strcpy(history[i], history[i+1]);
			  if (ngram > 1) strcpy(history[i], word);
		  }
		  else {
			  if (ngram > 1) strcpy(history[n], word);
		  	  //printf("Copiamos %s en history[%d]\n", word, n); fflush(stdout);
			  n++;
		  }
	  }
  }

  fclose(fd_test);

  return table;

}

int wordsintest(char *kgram, char **vocabulary, h_t *test, char c2, int ngram, char **history, int type) {
  char *next, *aux, *ending;
  char word[MAXLINE], key[MAXLINE];
  int n, length, i = 0, j, k;

  next = kgram;
  while (next != NULL) {
	  if (*next == '<') {
		  sscanf( (const char *) next, "%s", word);
		  if (!strcmp(word, "<s>")) {
			  if (ngram > 1) {
				strcpy(history[i], word);
		  	  	i++;
			  }
		  }
		  else if (!strcmp(word, "</s>")) {
			for (k = i-1; k>= 0; k--) {
	  			strcpy(key, history[k]);
				for (j = k+1; j < i; j++)
		  			sprintf(key, "%s %s", key, history[j]);
	  			strcat(key, " ");
	  			strcat(key, word);

  				//fprintf(stderr, "%s: Buscando n-gram %s => ", kgram, key);

  				if (hs(key, strlen(key)+1, test) == (void *) -1) return 1;
				//fprintf(stderr, "Succeed\n");
			}
		  }
	  }
	  else {
		  if (type == 1) {
		  	sscanf( (const char *) next, "%d", &n);
		  	aux = vocabulary[n];
		  }
		  else aux = next;
		
		  while (aux != NULL) {
			  ending = strchr(aux, c2);
			  if (ending != NULL) {
				  length = ending - aux;
				  strncpy(word, aux, length);
				  word[length] = '\0';
				  aux = ending + 1;
			  }
			  else {
				  sscanf( (const char *) aux, "%s", word);
				  //printf("Palabra => %s\n", word);
				  aux = NULL;
			  }

	  		  //fprintf(stderr, "\tBuscando unigrama %s =>\n", word);
			  if (hs(word, strlen(word)+1, test) == (void *) -1) return 1; /* Unigram Filtering */
			  else {
				  //fprintf(stderr, "\t\tUnigram Succeed\n");
				  for (k = i-1; k>= 0; k--) {
	  				strcpy(key, history[k]);
					for (j = k+1; j < i; j++)
		  				sprintf(key, "%s %s", key, history[j]);
	  				strcat(key, " ");
	  				strcat(key, word);

	  				//printf("%s: Buscando n-gram %s => ", kgram, key);

	  				if (hs(key, strlen(key)+1, test) == (void *) -1) return 1;
					//printf("Succeed\n");
				  }
			  }

		  	  if (i >= ngram - 1) {
				  for (j = 0 ; j < ngram-2; j++)
					  strcpy(history[j], history[j+1]);
			  	  if (ngram > 1) strcpy(history[j], word);
		  	  }
			  else {
				  strcpy(history[i], word);
				  //printf("Copiamos %s en history[%d]\n", word, i); fflush(stdout);
				  i++;
		  	  }

		  }
	  }

	  next = strchr(next, ' ');
	  if (next != NULL) next++;
  }
  //fprintf(stderr, "K-gram succeed\n");
  return 0; /* No hay que filtrar */
}

/********* End::: FILTRADO DEL MODELO MEDIANTE EL CORPUS DE TEST *********/

/********* Begin::: VOCABULARIO EXTENDIDO *********/

/* Lectura del vocabulario extendido
   ---------------------------------
   Lee la informacion desde fichero
   El vocabulario está codificado mediante números

   Retorna el vocabulario como un array de palabras de tamaño size en:
   ---> vocabulary[0]      = (sin usar)
        vocabulary[1..|V|] = simbolos extendidos
        ---> donde el carácter de control c1 se sustituye por un espacio en blanco
        vocabulary[size-3] = NULL
        vocabulary[size-2] = "BACKOFF"
        vocabulary[size-1] = "<unk> <unk>" ("<unk> <unk> ({0})" en factored-SMT)

   Retorna las probabilidades de los símbolos extendidos como un array de estructuras en:
   ---> local[0]           = (sin usar)
        local[1..|V|]      = cada estructura contiene un array de probabilidades
   Retorna el número de estados intermedios que hacen falta por arista (en promedio)
*/
int get_vocabext(char *vocabext, int size, char c1, char c2, int algorithm, char ***vocabulary, lexicon **local) {
  FILE *fd_vocabext;
  int i, j, n, max = 0, trg = 0, total, nae;
  char line[MAXLINE], word[MAXLINE], *p;
  double avgsrc, prob;

  if ((fd_vocabext = fopen(vocabext, "r")) == NULL ) {
	  fprintf(stderr, "Failed open: extended vocabulary file <%s>\n", vocabext);
	  exit(EXIT_FAILURE);
  }

  memtest(*vocabulary = (char **) malloc(size*sizeof(char *)));
  memtest(*local = (lexicon *) malloc((size-3)*sizeof(lexicon)));

  //while (fscanf(fd_vocabext, "%d %s", &i, word) != EOF) {
  while (fgets(line, MAXLINE, fd_vocabext) != NULL) {
	  j = 0;
	  sscanf(line, "%d %s", &i, word);
	  p = line;
	  p = strchr(p, ' '); p++; p = strchr(p, ' ');
	  if (p != NULL) {
	    do {
		j++;
		sscanf(p, "%lf", &prob);
		if (prob > 0) (*local)[i].p[j] = -log10(prob);
		else (*local)[i].p[j] = INT_MAX;
		p++;
		p = strchr(p, ' ');
	    } while (p != NULL);
	    (*local)[i].n = j;
	  }
	  else {
		free(*local);
		*local = NULL;
	  }
	  /* fprintf(stderr, "%d ", i);
	  for (j = 1; j <= (*local)[i].n; j++) fprintf(stderr, "%f ", (*local)[i].p[j]);
	  fprintf(stderr, "\n"); */

	  p = word;
	  while ((p = strchr(p, c1)) != NULL) {
		*p = ' ';
		trg++;
	  }

	  n = 0;
	  p = word;
	  while ((p = strchr(p, c2)) != NULL) {
		  n++;
		  p++;
	  }
	  max += n;

	  memtest((*vocabulary)[i] = strdup(word));
  }

  (*vocabulary)[size - 3] = NULL;
  
  sprintf(word, "<BACKOFF>");
  memtest((*vocabulary)[size - 2] = strdup(word));
  
  if ((algorithm == 1) || (algorithm == 3)) sprintf(word, "<unk> <unk>");
  else sprintf(word, "<unk> <unk> ({0})");
  memtest((*vocabulary)[size - 1] = strdup(word));

  fclose(fd_vocabext);

  total = size - 4;
  avgsrc = ((double) (max + total)) / total;
  /* avgtrg = (double) trg / total;
  ratio = avgtrg / avgsrc; */

  //fprintf(stderr, "Longitud media de los simb. extendidos = %lf\n", avgsrc);
  nae = ceil(avgsrc - 1);

  return nae;

}

void free_vocabext(char **vocabulary, int size) {
  int i, ending;

  ending = size - 3;
  for (i = 1; i < size; i++)
	  if (i != ending) free(vocabulary[i]);
  free(vocabulary);
}

/********* End::: VOCABULARIO EXTENDIDO *********/

int get_info(char *line, int n, int ngram, double *prob, char *kgram, int size, int id, double *BO, char *left, char *right,
	     int *last, char *symbol, int *word_id, struct options *opt) {
  int i, info;
  char word[MAXLINE];
  char *next;
  
  if ((*line != '\\') && (*line != '\0')) {
  	sscanf( (const char *) line, "%lf", prob);
	*prob = -(*prob);
  	next = strchr(line, '\t');
  	next++;

  	strcpy(left, "");
  	strcpy(kgram, "");
  	strcpy(right, "");
  	for (i = 1; i <= n; i++) {
	  sscanf( (const char *) next, "%s", word);
	  strcat(kgram, word);

	  if (i > 1) strcat(right, word);
	  if (i < n) {
	  	strcat(left, word);
		strcat(kgram, " ");
		if (i > 1) strcat(right, " ");
	  	next = strchr(next, ' ');
	  	next++;
	  }
	  if (i < n-1) strcat(left, " ");
	  if (i == n) {
		  strcpy(symbol, word);
		  if ((strcmp(word, "<s>") != 0) && (strcmp(word, "<unk>") != 0) && (strcmp(word, "</s>") != 0)) {
			if (opt->type == 1) *last = atoi(word);
			else *last = *word_id;
			if (n == 1) (*word_id)++;
		  }
		  else if (i == 1) {
			  if      (!strcmp(word, "</s>"))   *last = size - 3;
			  else if (!strcmp(word, "<s>"))    *last = size - 2;
			  else if (!strcmp(word, "<unk>"))  *last = size - 1;
		  }
	  }
  	}

  	if (n < ngram) {
	  next = strchr(next, '\t');
	  if (next != NULL) {
	  	next++;
	  	sscanf( (const char *) next, "%lf", BO);
		*BO = -(*BO);
	  }
	  else *BO = 0;
  	}

	if (n == 1) return *last;
	else return id;
  } 
  else return 0;
}

int validate_transition(int type, struct options *opt, lexicon *local) {
  int i;

  if ((type == Unknown) || (local == NULL)) return 1;
  else for (i = 1; i <= local->n; i++)
	if ((local->p[i] == INT_MAX) && (opt->l[i] != 0) && (opt->l[i] != 1)) return 0;
  return 1;
}

void intro_transition(T_REDGEN *v2, int type,
  int source, int target, char *symbols, double weight, int *id, char c2, int stage, struct options *opt,
		      lexicon *local) {
  int src, i;
  double w;
  char word[MAXLINE], key[MAXLINE], *p1, *p2;
  lexicon l, *p;
  int *word_id;
  T_ARISTA *a;

  src = source;
  //w = weight;
  p1 = symbols;
  p = &l;
  if (local != NULL) {
	for (i = 1; i <= local->n; i++) l.p[i] = 0.0;
	l.n = local->n;
  }
  else p = NULL;

  //printf("Buscando %c sobre %s\n", c2, p1); fflush(stdout);

  if (validate_transition(type, opt, local)) {
  
     while ((p2 = strchr(p1, c2)) != NULL) {
	  strncpy(word, p1, p2 - p1);
	  word[p2 - p1] = '\0';

          word_id = hs(word, p2-p1+1, v2->table_in);
	  a = (T_ARISTA *) -1;
          if (word_id != (int *) -1) {
		sprintf(key, "%d %d", src, *word_id);
          	a = hs(key, strlen(key)+1, v2->table);
		if (a != (T_ARISTA *) -1) {
			while (a != NULL) {
				if (v2->est[a->dest].BACKOFF == NULL) break;
				a = a->siguiente;
			}
		}
	  }

          if ((word_id != (int *) -1) && (a != (T_ARISTA *) -1) && (a != NULL))
          	src = a->dest;
	  else {

	  //fprintf(stderr, "Intermedios %d => %d ... %s/%lf\n", src, *id, word, w); fflush(stderr);
	  	enter_state(v2, *id, INT_MAX, INT_MAX);
	  	enter_transition(v2, stage, type, src, *id, word, 0.0, p);
	  	//enter_transition(v2, stage, type, src, *id, word, w, p);

	  	src = *id;
	  	(*id)++;
          }
	  //w = 0.0;
	  //if (local != NULL) for (i = 1; i <= local->n; i++) l.p[i] = 0.0;
	  p1 = p2 + 1;
     }

     //fprintf(stderr, "Construyendo %d => %d ... %s/%lf\n", src, target, p1, weight); fflush(stderr);
     enter_transition(v2, stage, type, src, target, p1, weight, local);
  }
}

T_REDGEN *arpa2fsm(char *prefix, char *TEST,
		   int filtering, int hts, char *separator, int algorithm, char c1, char c2, int stage,
		   struct options *opt) {
  char vocabext[MAXCONF], arpa[MAXCONF], fsm[MAXCONF];
  FILE *fd_arpa;
  char line[MAXLINE], kgram[MAXLINE], left[MAXLINE], right[MAXLINE], word[MAXLINE], symbol[MAXLINE];
  int state = 0;
  int n = 0, ngram;
  double prob, BO, *acumprob;
  double ini, fin;
  int id, number, *index, *l_id, *r_id;
  h_t *tableOLD, *tableNEW, *test;
  int HASH_SIZE = 1;
  int GLOBAL_HASH_SIZE = 1;
  int acum = 0, type, max = 0;
  T_REDGEN *v2;
  char **vocabulary, **history;
  int size, backoff, unknown;
  int last, nae = 0;
  int i, filter;
  he_t *p;
  int cont = 0, count = 0;
  int from, word_id = 1;
  char *start, *pt;
  lexicon *local = NULL, *ptr;

  //printf("%s %s %d %d %s %d %c %c\n", prefix, TEST, filtering, hts, separator, algorithm, c1, c2);


  /* for(i=0;i<test->hsize;i++)
      for (p=test->htable[i];p!=NULL;p=p->p)
            printf("%s\n", p->key); */

  sprintf(arpa, "%s.arpa", prefix);
  if ((fd_arpa = fopen(arpa, "r")) == NULL ) {
	fprintf(stderr, "Failed open: arpa file <%s>\n", arpa);
	exit(EXIT_FAILURE);
  }

  while (fgets(line, MAXLINE, fd_arpa) != NULL) {
	  line[strlen(line)-1]='\0';

	  switch (state) {
		  case 0: if (!strcmp(line, "\\data\\")) state = 1;
			  break;
	  	  case 1: if (sscanf( (const char *) line, "ngram %d=%d", &ngram, &number) < 2) state = 2;
			  else {
			    if (ngram == 1) {
				size = number + 1;
				id = size;
				if (opt->type == 1) {
  				   sprintf(vocabext, "%s.vocabext", prefix);
				   nae = get_vocabext(vocabext, size, c1, c2, algorithm, &vocabulary, &local);
				}
  				else {
					memtest(vocabulary = (char **) malloc(size*sizeof(char *)));
  					vocabulary[size - 3] = NULL;
  
  					sprintf(word, "<BACKOFF>");
  					memtest(vocabulary[size - 2] = strdup(word));
  
  					if ((algorithm == 1) || (algorithm == 3)) sprintf(word, "<unk> <unk>");
  					else sprintf(word, "<unk> <unk> ({0})");
  					memtest(vocabulary[size - 1] = strdup(word));
				}
			    }
			    acum+=number;
			    if (number > max) max = number;
			    break;
			  }
		  case 2: if (strstr(line, "-grams:") != NULL) {
			    if (ngram == 1) id = 1;
  			    if (filtering) {
				   history = (char **) malloc((filtering - 1) * sizeof(char *));
				   for (i = 0; i < filtering-1; i++)
					   history[i] = (char *) malloc(MAXLINE * sizeof(char));
				   test = load_test(TEST, hts, separator, filtering, history);
			    }
			    while (GLOBAL_HASH_SIZE < max)
			  	GLOBAL_HASH_SIZE = GLOBAL_HASH_SIZE * 2;
  			    tableOLD = hc(GLOBAL_HASH_SIZE);
  			    memtest(index = (int *) malloc(sizeof(int)));
  			    *index = 0;
  			    hi("", 1, index, tableOLD);

			    tableNEW  = hc(GLOBAL_HASH_SIZE);

			    /* fprintf(stderr, "Numero de estados del automata = %d\n", acum-number+1);
			    fprintf(stderr, "Longitud media de los simbolos extendidos = %d\n", nae+1);
			    fprintf(stderr, "Numero total de transiciones = %d\n", acum); */
			    acum = acum - number + 1 + nae*acum;
			    while (HASH_SIZE < size)
			  	HASH_SIZE = HASH_SIZE * 2;

			    //fprintf(stderr, "Creando red con %d estados\n", acum);
			    v2 = CrearRed(acum, HASH_SIZE);
			    sprintf(v2->nombre, "%d-grams_transducer", ngram);
			    if (ngram == 1) enter_state(v2, 0, 0, INT_MAX);
			    else enter_state(v2, 0, INT_MAX, INT_MAX);

			    n++;
			    state = 3;
			  }
			  break;
		  case 3: if (strstr(line, "-grams:") == NULL) { 	/* Recorrer las n-gramas */
			    //printf("Primer id n>1 o artificiales => %d\n", size); fflush(stdout);
			    number = get_info(line, n, ngram, &prob, kgram, size, id, &BO, left, right, &last, word, &word_id, opt);
			    if (number) {
			      if (opt->type == 2) {
				sprintf(symbol, "%s %s", word, word);
			    	if ((n == 1) && (last < size-3)) memtest(vocabulary[last] = strdup(symbol));
			      }

			    /**** AQUI HAY QUE COMPROBAR si todas las palabras de input del kgram estan en el test ****/

			      //fprintf(stderr, "Examinando %s\n",
			      //	      	kgram); fflush(stderr);
			      if (filtering) filter= wordsintest(kgram, vocabulary, test, c2, filtering, history, opt->type);
			      else filter = 0;
			      //fprintf(stderr, "Hay que filtrar = %d\n", filter); fflush(stderr);
			      count++;
			      if (!filter) {
				if (n < ngram) {
					memtest(index = (int *) malloc(sizeof(int)));
					*index = number;
					//fprintf(stderr, "Insertando %s (%d)\n", kgram, number); fflush(stderr);
					hi(kgram, strlen(kgram)+1, index, tableNEW); 
					//fprintf(stderr, "kk2\n"); fflush(stderr);
				}
				l_id = hs(left, strlen(left)+1, tableOLD);
				r_id = hs(right, strlen(right)+1, tableOLD);
				if ((l_id != ((int *) -1)) && (r_id != ((int *) -1))) {
				   cont++;

				   if (strstr(kgram, "</s>") == NULL) {
  				      if (n < ngram) {
				    	if ((n == 1) && (!strcmp(kgram, "<s>"))) ini = 0;
				    	else ini = INT_MAX;

				        //fprintf(stderr, "Ponemos el estado %d\n", number); fflush(stderr);
		  		    	enter_state(v2, number, ini, INT_MAX);

				    	backoff = size - 2;
				    	enter_transition(v2, stage, Backoff, number, *r_id, vocabulary[backoff], BO, NULL);
					//Añadir arista de bajada por backoff (log 10)
							
					//fprintf(stderr, "\t%lf", BO);
					//fprintf(stderr, "\t\t BACKOFF (%d) => (%d)\n", number, *r_id); fflush(stderr);

					//Añadir arista de subida (log 10)
						
				    	if (strcmp(kgram, "<s>") != 0) {
				      	   if (!strcmp(kgram, "<unk>")) {
						type = Unknown;
						last = size - 1;
				      	   }
				      	   else type = Transit;

				      	   if (n > 1) id++;
				      	   if ((local != NULL) && (type == Transit)) ptr = &local[last];
				      	   else ptr = NULL;
				      	   if (opt->type == 1) strcpy(symbol, vocabulary[last]);
				      /* if (n > 1) printf("\tType=%d (%d)===%s===%lf===>(%d)\n",
					        type, *l_id, symbol, prob, number);
				      fflush(stdout); */
				      	   intro_transition(v2, type, *l_id, number, symbol, prob, &id, c2, stage, opt, ptr);
				   	}
					//if (n > 1) id++;
				     }
				     else {
				    	if (strcmp(kgram, "<s>") != 0) {
				      	   if (!strcmp(kgram, "<unk>")) {
					   	type = Unknown;
					   	last = size - 1;
				           }
				           else type = Transit;
				      //Añadir arista entre estados del mismo nivel (log 10)
				           if ((local != NULL) && (type == Transit)) ptr = &local[last];
				           else ptr = NULL;
				           if (opt->type == 1) strcpy(symbol, vocabulary[last]);
					   intro_transition(v2, type, *l_id, *r_id, symbol, prob, &id, c2, stage, opt, ptr);
				        }
				     }  
				  }
				  else {
				     if (n < ngram) {
					enter_state(v2, number, INT_MAX, INT_MAX);
				  	if (n > 1) id++;
				     }
				     v2->est[*l_id].final = prob;
				  }
				}
			      }
			    }
			    else state = 4;
			  }
			  break;
		  case 4: fprintf(stderr, "%d: %d/%d\n", n, cont, count);
			  //if (n==3) exit(1);
                          //fprintf(stderr, "\nVocabulario de %d palabras\n", size-1);
			  fflush(stderr);
			  //for (i = 1; i < size; i++) printf("%d %s\n", i, vocabulary[i]); fflush(stdout);
			  cont = count = 0;
			  //contkk = contkk1 = contkk2 = 0;

			  hd(tableOLD, 1);
  			  if (n < ngram) {
				tableOLD = tableNEW;
			  }
			  n++;
			  if (n < ngram) tableNEW  = hc(GLOBAL_HASH_SIZE);
		  	  if (strstr(line, "-grams:") != NULL) state = 3;
			  break;
	  }
  }

  v2->ne = id;
  //printf("Id = %d\n", id);

  if (filtering) {
  	for (i = 0; i < ngram-1; i++)
	  free(history[i]);
  	free(history);
  }

  fclose(fd_arpa); 

  free_vocabext(vocabulary, size);
  if (filtering) hd(test, 0);

  get_vocabulary(v2, IN);
  get_vocabulary(v2, OUT);
  get_final_probs(v2);

  return v2;
}
