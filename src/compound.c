#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "giati.h"
#include "compound.h"

void search_characters(struct options *opt, char *c1, char *c2,
                                            char *c3, char *c4) {
  int simbols[256];
  char corpus[MAXCONF];
  FILE *fd_corpus;
  unsigned char c;
  int i;

  for (i = 0; i < 256; i++) simbols[i] = 0;
  sprintf(corpus, "%s/%s", opt->corpus_dir, opt->corpus);
  if ((fd_corpus = fopen(corpus, "r")) == NULL ) {
         fprintf(stderr, "Failed open: corpus file <%s>\n", corpus);
         exit(EXIT_FAILURE);
  }
  while (fscanf(fd_corpus, "%c", &c) != EOF) simbols[c] = 1;
  fclose(fd_corpus);

  *c1 = *c2 = *c3 = *c4 = 0;
  /* for (i = 161; i < 256; i++)
          if (!simbols[i]) {
                if (!*c1) *c1 = i;
                else if (!*c2) *c2 = i;
                else if (!*c3) *c3 = i;
                else if (!*c4) *c4 = i;
          } */
  for (i = 33; i < 127; i++)
          if ((i != 60) && (i != 62) && (!simbols[i])) {
                if (!*c1) *c1 = i;
                else if (!*c2) *c2 = i;
                else if (!*c3) *c3 = i;
                else if (!*c4) *c4 = i;
          }
}

void build_compound_sentence(char *source, int sourcel, char *target, int targetl, char separator,
                             int *alignments, int algorithm, int simbextmax, char *output) {
  int ids = 1, idt = 1;
  int n_out;
  int origen;

  /* printf("Numero de palabras source/target = %d/%d\n", sourcel, targetl);
  for (i = 1; i <= targetl; i++) printf("%s(%d)\n", &target[i*MAXWORDLENGTH], alignments[i]);
  for (i = 1; i <= sourcel; i++) printf("%s\n", &source[i*MAXWORDLENGTH]);

  sprintf(line, "");
  for (i = 1; i < sourcel; i++) sprintf(line, "%s%s ", line, &source[i*MAXWORDLENGTH]);
  strcat(line, &source[sourcel*MAXWORDLENGTH]); */

  strcpy(output, "");
  while (ids <= sourcel) {
	strcat(output, &source[ids*MAXWORDLENGTH]);

	n_out = 1;
	while ((idt <= targetl) && (alignments[idt] <= ids)) {
		if (n_out <= simbextmax) {
			sprintf(output, "%s%c%s", output, separator, &target[idt*MAXWORDLENGTH]);
			if (algorithm == 2) {
				if (alignments[idt] == 0) origen = -1;
				else origen = ids - alignments[idt];
				sprintf(output, "%s%c({%d})", output, separator, origen);
			}
			n_out++;
			idt++;
		}
		else break;
	}

	if (ids < sourcel) strcat(output, " ");
	ids++;
  }
  while (idt <= targetl) {
          //printf("KK:%s\n", output); exit(1);
	  sprintf(output, "%s%c%s", output, separator, &target[idt*MAXWORDLENGTH]);
	  if (algorithm == 2) {
		if (alignments[idt] == 0) origen = -1;
		else origen = ids - alignments[idt] - 1;
		sprintf(output, "%s%c({%d})", output, separator, origen);
	  }
	  idt++;
  }
}

void build_minimum_segmentation(char *source, int sourcel, char *target, int targetl, char c1, char c2,
				int *alignments, int algorithm, char *output) {
  int i, j, k, start, stop;
  segment *vector;
  int ids = 1, idt = 1;
  int origen;

  vector = (segment *) malloc((sourcel + 1) * sizeof(segment));
  for (i = 1; i <= sourcel; i++) {
	vector[i].start = targetl+1;
	vector[i].stop  = 0;
  }
  for (j = 1; j <= targetl; j++) {
	vector[alignments[j]].start = targetl+1;
	vector[alignments[j]].stop  = 0;
  }

  //for (i = 1; i <= sourcel; i++) printf("%d %d\n", vector[i].start, vector[i].stop);

  for (j = 1; j <= targetl; j++) {
	if (j < vector[alignments[j]].start) vector[alignments[j]].start = j;
	if (j > vector[alignments[j]].stop)  vector[alignments[j]].stop  = j;
  }

  //for (i = 1; i <= sourcel; i++) printf("%d %d\n", vector[i].start, vector[i].stop);

  for (i = 1; i <= sourcel; i++)
  	for (j = i+1; j <= sourcel; j++)
		if (vector[i].stop > vector[j].start) {
			if (vector[i].start < vector[j].start) start = vector[i].start;
			else start = vector[j].start;
			if (vector[i].stop  < vector[j].stop)  stop  = vector[j].stop;
			else stop  = vector[i].stop;
			for (k = i; k <= j; k++) {
				vector[k].start = start;
				vector[k].stop  = stop;
			}
		}

  for (i = 1; i <= sourcel; i++)
	if (vector[i].stop == 0) {
		for (j = i+1; j <= sourcel && vector[j].stop == 0; j++);
		if (j <= sourcel) {
			vector[i].start = vector[j].start;
			vector[i].stop  = vector[j].stop;
		}
		else if (i > 1) {
			vector[i].start = vector[i-1].start;
			vector[i].stop  = vector[i-1].stop;
		}
		else {
			vector[i].start = 1;
			vector[i].stop  = targetl;
		}
	}

  //for (i = 1; i <= sourcel; i++) printf("%d %d\n", vector[i].start, vector[i].stop);

  strcpy(output, "");
  for (i = 1; i <= sourcel; i++) {
	start = vector[i].start;
	stop  = vector[i].stop;
	strcat(output, &source[i*MAXWORDLENGTH]);
	ids = i + 1;
  	while ((ids <= sourcel) && (vector[ids].start == start) && (vector[ids].stop == stop)) {
		sprintf(output, "%s%c%s", output, c2, &source[ids*MAXWORDLENGTH]);
		ids++;
  	}
	for (k = idt; k <= stop; k++) {
		sprintf(output, "%s%c%s", output, c1, &target[k*MAXWORDLENGTH]);
		if (algorithm == 4) {
			if (alignments[k]==0) origen = -1;
			else origen = ids - 1 - alignments[k];
			sprintf(output, "%s%c({%d})", output, c1, origen);
		}
	}
	idt = k;
        if (ids <= sourcel) strcat(output, " ");
	i = ids - 1;
  }
  //printf("%s\n", output);
	 
  free(vector);
}

int auto_contained(int sourcel, int targetl, int **matrix, int jb, int je, int ib, int ie) {
//int auto_contained(int sourcel, int targetl, int **matrix, 4 10 4 11) {
  int i, j; 

  for (j = jb+1; j <= je; j++) {
    for (i = 1; i <= targetl; i++) {
      if ((i <= ib) || (i > ie))
        if (matrix[i][j] == 1) return 0;
    }
  }

  for (i = ib+1; i <= ie; i++) {
    for (j = 1; j <= sourcel; j++) {
      if ((j <= jb) || (j > je))
        if (matrix[i][j] == 1) return 0;
    }
  }
  
  return 1;
}

void build_alls(char *source, int sourcel, char *target, int targetl, char c1, char c2,
	        int **matrix, char *output) {
  int source_start = 0, target_start= 0;
  int i, j, k, min;
  int jb, je, ib, ie;
  int flag;
  char line[MAXLINE];
  sa **lea;
  sa *nodo, *aux, *ant;

  /* for (i = targetl; i > 0; i--) {
    for (j = 1; j <= sourcel; j++)
	printf("%d ", matrix[i][j]);
    printf("\n");
  }
  exit(1); */

  if (sourcel < targetl) min = sourcel;
  else min = targetl;

  lea = (sa **) malloc(min * sizeof(sa *));
  lea[0] = (sa *) malloc(sizeof(sa));
  lea[0]->j = lea[0]->i = 0;
  lea[0]->ant = NULL;
  lea[0]->sig = NULL;

  for(k = 0; k < min; k++) {
    //printf("Etapa %d\n", k);
    lea[k+1] = NULL;
    nodo = lea[k];
    ant = NULL;
    while (nodo != NULL) {
      source_start = nodo->j;
      target_start = nodo->i;
      //printf("Explorando (%d %d)\n", nodo->j, nodo->i);
      if ((source_start == sourcel) && (target_start == targetl)) {
	aux = (sa *) malloc(sizeof(sa));
        aux->j = source_start;
        aux->i = target_start;
	aux->ant = nodo;
        if (ant == NULL) lea[k+1] = aux;
	else ant->sig = aux;
	ant = aux;
      }
      else {
        for (j = 1; j <= MAX_PHRASE_LENGTH && source_start + j <= sourcel; j++) {
          for (i = 1; i <= MAX_PHRASE_LENGTH && target_start + i <= targetl; i++) {
	    if (auto_contained(sourcel, targetl, matrix, source_start, source_start + j, target_start, target_start + i)) {
	      aux = (sa *) malloc(sizeof(sa));
	      aux->j = source_start + j;
	      aux->i = target_start + i;
	      aux->ant = nodo;
              if (ant == NULL) lea[k+1] = aux;
	      else ant->sig = aux;
	      ant = aux;
	      //printf("(%d %d)\n", source_start + j, target_start + i);
	    }
          }
        }
      }
      nodo = nodo->sig;
    }
  }

  nodo = lea[min];
  while (nodo != NULL) {
    je = nodo->j; ie = nodo->i;
    //printf("Explorando (%d %d)\n", je, ie);
    if ((je == sourcel) && (ie == targetl)) {
      ant = nodo->ant;
      jb = ant->j; ib = ant->i;
      while ((jb == sourcel) && (ib == targetl)) {
	ant = ant->ant;
        jb = ant->j; ib = ant->i;
      }
      //printf("Source (%d %d) => Target (%d %d)\n", jb, je, ib, ie);
      strcpy(output, ""); flag = 0;
      do {
	if (flag) {
      	  sprintf(line, " %s", output);
      	  strcpy(output, line);
	}
      for (i = ie; i > ib; i--) {
        sprintf(line, "%c%s%s", c1, &target[i*MAXWORDLENGTH], output);
        strcpy(output, line);
      }
      sprintf(line, "%s%s", &source[je*MAXWORDLENGTH], output);
      strcpy(output, line);
      for (j = je - 1; j > jb; j--) {
        sprintf(line, "%s%c%s", &source[j*MAXWORDLENGTH], c2, output);
        strcpy(output, line);
      }
	je = jb; ie = ib;
	ant = ant->ant;
        if (ant != NULL) {
	  jb = ant->j;
	  ib = ant->i;
	}
	flag = 1;
      //printf("Source (%d %d) => Target (%d %d)\n", jb, je, ib, ie);
      } while (ant != NULL);
      printf("%s\n", output);
    }
    nodo = nodo->sig;
    //exit(1);
  }
}

void write_extended_simbols(struct options *opt, char c1, char c2) {
  char align[MAXCONF], extended[MAXCONF];
  FILE *fd_align, *fd_extended;
  char line[MAXLINE];
  char source[MAXWORDS][MAXWORDLENGTH];
  char target[MAXWORDS][MAXWORDLENGTH];
  int alignments[MAXWORDS];
  int** matrix;
  int cont = 0, sourcel, targetl;
  int i, j, number;
  char *next;

  sprintf(align, "%s/%s", opt->corpus_dir, opt->align);
  sprintf(extended, "%s/%s%s.compost", opt->work_dir, opt->prefix, opt->corpus);

  if ((fd_align = fopen(align, "r")) == NULL ) {
         fprintf(stderr, "Failed open: alignments file <%s>\n", align);
         exit(EXIT_FAILURE);
  }

  if ((fd_extended = fopen(extended, "w")) == NULL ) {
         fprintf(stderr, "Failed write: extended file <%s>\n", extended);
         exit(EXIT_FAILURE);
  }

  while (fgets(line, MAXLINE, fd_align) != NULL) {
  	line[strlen(line)-1]='\0';
	if (cont == 0) {
	  sscanf( (const char *) line,
          "# Sentence pair (%d) source length %d target length %d", &i, &sourcel, &targetl);
	  matrix = (int **) malloc((targetl + 1)*sizeof(int *));
	  for (i = 0; i <= targetl; i++) matrix[i] = (int *) malloc((sourcel + 1)*sizeof(int));
          for (i = 1; i <= targetl; i++)
	    for (j = 0; j <= sourcel; j++)
              matrix[i][j] = 0;
	}
	else if (cont == 1) {
		next = line;
		for (i = 1; i <= targetl; i++) {
			sscanf( (const char *) next, "%s", target[i]);
			next = strchr(next, ' ');
			next++;
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
				matrix[number][i] = 1;
				next = strchr(next+1, ' ');
			}
			sscanf( (const char *) next, " }) ");
			next = strchr(next+1, ' ');
			next++;
		}

  		switch (opt->algorithm) {
			case 1:case 2: build_compound_sentence(&source[0][0], sourcel, &target[0][0], targetl, c1,
				                               alignments, opt->algorithm, opt->simbextmax, line);
				       break;
			case 3:case 4: build_minimum_segmentation(&source[0][0], sourcel, &target[0][0], targetl, c1, c2,
							       alignments, opt->algorithm, line);
				       break;
			case 5:case 6: build_alls(&source[0][0], sourcel, &target[0][0], targetl, c1, c2, matrix, line);
				       break;
		}
		fprintf(fd_extended, "%s\n", line);
	}

	cont = (cont + 1)%3;
  }
  fclose(fd_align);
  fclose(fd_extended);
  if (opt->algorithm >=5) exit(EXIT_FAILURE);
}
