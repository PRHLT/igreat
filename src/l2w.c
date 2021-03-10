#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fsm.h"
/* #include "giati.h" */
#include "l2w.h"

l_int *read_entry(l_int *output, char **dict, char **tags, int n, h_t *table, h_t *vocab) {
  char lemma[MAXLINE], alignment[MAXLINE];
  char key[MAXLINE];
  int *index;
  int align;
  l_int *aux, *new = NULL;
  int pos;
  int ident, *index_wrd;

  aux = output;
  while (aux != NULL) {
	  ident = aux->n;
	  strcpy(lemma, dict[aux->n]);
	  aux = aux->next;
	  strcpy(alignment, dict[aux->n]+2);
	  *(strstr(alignment, "})")) = '\0';
	  align = atoi(alignment);
	  aux = aux->next;

	  if (align >= 0) pos = n - align;
	  else pos = -1;
	  if ((pos > 0) && (pos <= n))
	  	sprintf(key, "%s %s", lemma, tags[pos]);
	  else sprintf(key, "%s NULL", lemma);
	  //printf("Key = (%s) => ", key);

  	  index = hs(key, strlen(key)+1, table);
  	  if ((index == (int *) -1) && (pos > 0) && (pos <= n)) {
		  //printf("KK");
		  sprintf(key, "%s NULL", lemma);
  	  	  index = hs(key, strlen(key)+1, table);
	  }

  	  if (index != (int *) -1) {
		  //printf("Localizada %s ...", index);
		  /* index_wrd = hs(index, strlen(index)+1, vocab);
		  ident = -(*index_wrd); */
		  ident = -(*index);
		  //printf("IMPACTO %d\n", ident);
	  }
	  //else printf("AGUA %d\n", ident);

	  new = insert(new, ident, NULL);
  }

  return new;
}

char **read_table(struct options *opt, h_t **table, h_t **vocab) {
  FILE *fd;
  char line[MAXLINE];
  char word[MAXWORDLENGTH], *end;
  int *index, id = 1, k;
  he_t *p;
  char **alfa;

  *table = hc(opt->hts);
  *vocab = hc(opt->hts);

  if ((fd = fopen(opt->table, "r")) == NULL ) {
        fprintf(stderr, "Failed open: lemma-word file <%s>\n", opt->table);
        exit(EXIT_FAILURE);
  }

  while (fgets(line, MAXLINE, fd) != NULL) {
	k = strlen(line);
  	line[k-1] = '\0';

	end = strrchr(line, ' ');
	strcpy(word, end + 1);
	*end = '\0';

	k = line+k - end - 1;
	index = hs(word, k, *vocab);
	if (index == (int *) -1) {
		memtest(index = (int *) malloc(sizeof(int)));
		*index = id;
		hi(word, k, index, *vocab);
	       	id++;
	}

	hi(line, end - line + 1, index, *table);

	/* data = strdup(word);
	hi(line, end - line + 1, data, *table);

	k = line+k - end - 1;
	index = hs(data, k, *vocab);
	if (index == (int *) -1) {
		memtest(index = (int *) malloc(sizeof(int)));
		*index = id;
		hi(data, k, index, *vocab);
	       	id++;
	}

	id++; */

  }

  fclose(fd);

  alfa = (char **) malloc(id * sizeof(char *));
  for(k = 0; k < (*vocab)->hsize; k++)
       	for (p = (*vocab)->htable[k]; p != NULL; p = p->p)
  		alfa[*((int *) p->data)] = p->key;

  return alfa;

}

void write_table(FILE *fd, h_t *table, char c1) {
  int i, j, *index, max;
  he_t *p, *pp;
  char semikey[MAXLINE], semiaux[MAXLINE], word[MAXLINE], *end;
  l_wordcount *aux;

  /* for(i = 0; i < table->hsize; i++)
       for (p = table->htable[i]; p != NULL; p = p->p)
		fprintf(fd, "%s %d\n", p->key, *((int *) p->data)); */

  //aux = hc(table->hsize);

  for(i = 0; i < table->hsize; i++)
       for (p = table->htable[i]; p != NULL; p = p->p) {
	      aux = p->data;
	      max = 0;
	      while (aux != NULL) {
		      if (aux->frequency > max) {
			      max = aux->frequency;
			      strcpy(word, aux->word);
		      }
		      aux = aux->next;
	      }
	      fprintf(fd, "%s %s\n", p->key, word);
       }
	


	       /* end = strrchr(p->key, c1);
	       strncpy(semikey, p->key, end - p->key);
	       semikey[end - p->key] = '\0';

	       strcpy(word, end + 1);
	       max = *((int *) p->data);

	       //printf("Semikey = %s\n\n", semikey);fflush(stdout);
	       
	       index = hs(semikey, strlen(semikey)+1, aux);
	       if (index == (int *) -1) {
		       hi(semikey, strlen(semikey)+1, NULL, aux);
		       for (j = i; j < table->hsize; j++)
			       for(pp = table->htable[j]; pp != NULL; ) {
			       	if (j == i) pp = p->p;
					if (pp != NULL) {
				       		end = strrchr(pp->key, c1);
				       		strncpy(semiaux, pp->key, end - pp->key);
				       		semiaux[end - pp->key] = '\0';
				       		//printf("--> Semiaux = %s\n", semiaux); fflush(stdout);
				       		if ((!strcmp(semikey, semiaux)) && (*((int *) pp->data) > max)) {
					       		//printf("Semikey = %s\nSemiaux = %s\n\n", semikey, semiaux);
							//fflush(stdout);
					       		max = *((int *) pp->data);
					       		strcpy(word, end + 1);
						}	
						pp = pp->p;
					}	
			       }	
		       *(strchr(semikey, c1)) = ' ';
		       fprintf(fd, "%s %s\n", semikey, word);
	       }


			       

	       //Si semikey no está en aux
	       //entonces
	       //introducir semikey en aux;
	       //buscar en el resto de entradas de la tabla
	       //para encontrar el máximo e imprimirlo

	       //*(strchr(semikey, c1)) = ' ';
	       //printf("%d %s %s\n", *((int *) p->data), semikey, word);
       } */

  //hd(aux, 0);
}


void get_l2w_table(struct options *opt, char c1) {
  char corpus[MAXCONF], align[MAXCONF], eagles[MAXCONF], tabfile[MAXCONF];
  FILE *fd_corpus, *fd_align, *fd_eagles, *fd_table;
  char line[MAXLINE];
  char words[MAXWORDS][MAXWORDLENGTH], tags[MAXWORDS][MAXWORDLENGTH], source[MAXWORDS][MAXWORDLENGTH];
  char wordt[MAXWORDS][MAXWORDLENGTH], tagt[MAXWORDS][MAXWORDLENGTH], target[MAXWORDS][MAXWORDLENGTH];
  int alignments[MAXWORDS];
  int cont = 0, sourcel, targetl;
  int i, number;
  char *next;
  h_t *table;
  he_t *p;
  int id;
  char key[MAXLINE];
  l_wordcount *aux, *prev;

  table = hc(opt->hts);

  sprintf(corpus, "%s/%s", opt->corpus_dir, opt->corpus);
  sprintf(align, "%s/%s", opt->corpus_dir, opt->align);
  sprintf(eagles, "%s/%s", opt->corpus_dir, opt->eagles);
  sprintf(tabfile, "%s/%s%s.tab", opt->corpus_dir, opt->prefix, opt->corpus);

  if ((fd_corpus = fopen(corpus, "r")) == NULL ) {
         fprintf(stderr, "Failed open: word corpus file <%s>\n", corpus);
         exit(EXIT_FAILURE);
  }

  if ((fd_align = fopen(align, "r")) == NULL ) {
         fprintf(stderr, "Failed open: alignments file <%s>\n", align);
         exit(EXIT_FAILURE);
  }

  if ((fd_eagles = fopen(eagles, "r")) == NULL ) {
         fprintf(stderr, "Failed open: eagles corpus file <%s>\n", eagles);
         exit(EXIT_FAILURE);
  }

  if ((fd_table = fopen(tabfile, "w")) == NULL ) {
         fprintf(stderr, "Failed write: lemma-word file <%s>\n", tabfile);
         exit(EXIT_FAILURE);
  }

  while (fgets(line, MAXLINE, fd_align) != NULL) {
  	line[strlen(line)-1]='\0';
	if (cont == 0) {
	  sscanf( (const char *) line,
          "# Sentence pair (%d) source length %d target length %d", &i, &sourcel, &targetl);
	}
	else if (cont == 1) {
		next = line;
		for (i = 1; i <= targetl; i++) {
			sscanf( (const char *) next, "%s", target[i]);
			next = strchr(next, ' ');
			next++;
			alignments[i] = 0;
		}
	}
	else if (cont == 2) {
		next = line;
		for (i = 0; i <= sourcel; i++) {
			sscanf( (const char *) next, "%s", source[i]);
			next = strchr(next, ' ');
			sscanf( (const char *) next, " ({ ");
			next = strchr(next+1, ' ');
			while (sscanf( (const char *) next, " %d", &number)) {
				alignments[number] = i;
				next = strchr(next+1, ' ');
			}
			sscanf( (const char *) next, " }) ");
			next = strchr(next+1, ' ');
			next++;
		}

  		fgets(line, MAXLINE, fd_corpus);
		line[strlen(line)-1]='\0';
		next = line;
		for (i = 1; i <= sourcel; i++) {
			sscanf( (const char *) next, "%s", words[i]);
			next = strchr(next, ' ');
			next++;
		}
		next = next + strlen(opt->separator) + 1;
		for (i = 1; i <= targetl; i++) {
			sscanf( (const char *) next, "%s", wordt[i]);
			next = strchr(next, ' ');
			next++;
		}

  		fgets(line, MAXLINE, fd_eagles);
		line[strlen(line)-1]='\0';
		next = line;
		for (i = 1; i <= sourcel; i++) {
			sscanf( (const char *) next, "%s", tags[i]);
			next = strchr(next, ' ');
			next++;
		}
		next = next + strlen(opt->separator) + 1;
		for (i = 1; i <= targetl; i++) {
			sscanf( (const char *) next, "%s", tagt[i]);
			next = strchr(next, ' ');
			next++;
		}

		strcpy(tags[0], "NULL");
		for (i = 1; i <= targetl; i++) {
			//sprintf(key, "%s%c%s%c%s", target[i], c1, tags[alignments[i]], c1, wordt[i]);
			sprintf(key, "%s %s", target[i], tags[alignments[i]]);
			aux = hs(key, strlen(key)+1, table);
			if (aux != (l_wordcount *) -1) {
				// Busquemos la palabra dentro de la lista
				// Si esta, entonces se incrementa el contador de frecuencias
				// si no, entonces Crear nodo con wordt[i] y 1
				// 		   Insertarlo al final de la lista

				prev = NULL;
				while (aux != NULL) {
					if (!strcmp(aux->word, wordt[i])) break;
					prev = aux;
					aux = aux->next;
				}
				if (aux != NULL) aux->frequency++;
				else {
					memtest(aux = (l_wordcount *) malloc(sizeof(l_wordcount)));
						aux->word = strdup(wordt[i]);
						aux->frequency = 1;
						aux->next = NULL;
					prev->next = aux;
				}
			}
			else {
				// Crear nodo con wordt[i] y 1 
				// Lista = nodo
				memtest(aux = (l_wordcount *) malloc(sizeof(l_wordcount)));
					aux->word = strdup(wordt[i]);
					aux->frequency = 1;
					aux->next = NULL;
				hi(key, strlen(key)+1, aux, table);
			}


		}
  		/* write_table(table); exit(1); */

		/*	HAY QUE CONSTRUIR LOS VECTORES source/target DE PALABRAS, LEMAS e EAGLES (lemas ya está)
			Recorrer la frase target
				Para cada posicion, obtener la codificación de Lt, Es, Wt = 1,2,3
				Incrementar la posicion Cube[1][2][3]++; */
				     

  		/* build_compound_sentence(&source[0][0], sourcel, &target[0][0], targetl, c1,
					alignments, opt->algorithm, opt->simbextmax, line);
		fprintf(fd_extended, "%s\n", line); */
	}

	cont = (cont + 1)%3;
  }

  write_table(fd_table, table, c1);

  fclose(fd_corpus);
  fclose(fd_align);
  fclose(fd_eagles);
  fclose(fd_table);

  for(i = 0; i < table->hsize; i++)
  	for (p = table->htable[i]; p != NULL; p = p->p) {
  		aux = p->data;
		while (aux != NULL) {
			prev = aux;
			aux = aux->next;
			free(prev->word);
			free(prev);
		}
	}

  hd(table, 0);

}
