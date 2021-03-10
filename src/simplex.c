#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "op.h"
#include "fsm.h"
#include "arpa.h"
#include "refx.h"
#include "l2w.h"
#include "parse.h"
#include "simplex.h"

#include <math.h>
#include "nrutil.h"
#define NDIM 4
#define TINY 1.0e-10
#define NMAX 5000
#define GET_PSUM \
		   for (j = 1; j <= ndim; j++) {\
			for (sum = 0.0, i = 1; i <= mpts; i++) sum += p[i][j];\
			psum[j] = sum;}
#define SWAP(a,b) { swap = (a); (a) = (b); (b) = swap; }

struct options options;
T_REDGEN *t = NULL, *a = NULL;
int *UNK;
FILE *fd_test;
ea **lea;

int prec_n (char **reference, int refsize, char **candidate, int size, int n, int *total) {
  int i, j, k, reftotal;
  int match, *matched;
  int prec;

  //printf("refsize = %d ||| size = %d ||| n = %d\n", refsize, size, n);

  if (n >= size) *total = 0;
  else *total = size - n;

  if (n >= refsize) reftotal = 0;
  else reftotal = refsize - n;

  matched = (int *) malloc((reftotal + 1) * sizeof(int));
  for (j = 1; j <= reftotal; j++) matched[j] = 0;

  prec = 0;
  for (i = 1; i <= *total; i++)
	for (j = 1; j <= reftotal; j++) {
		match = 1;
		for (k = 0; k < n; k++)
			if (strcmp(candidate[i+k], reference[j+k])) {
				match = 0;
				break;
			}
		if ((match) && (!matched[j])) {
			prec++;
			matched[j] = 1;
			break;
		}
	}
  return prec; 
}

float my_log(float x) {
	if (x == 0) return INT_MIN;
	else return log(x);
}

float funk(float lambda[]) {
  char line[MAXLINE], *middle;
  char source[MAXLINE], reference[MAXLINE], candidate[MAXLINE];
  char *next, c1, c2;
  int i, n, n_id, n1, n2;
  char words[MAXLINE], **sentence1, **sentence2;
  l_int *output;
  char **alfa, **target;
  float result, bp;
  int prec_sent, total_sent, precs[MAX_N], total[MAX_N];
  int refWords = 0, sysWords = 0;
/* Traducir el corpus de test mediante el transductor usando lambda1, lambda2, lambda3
   Calcular el BLEU y devolverlo mediante un return */

  if ((fd_test = fopen(options.test, "r")) == NULL ) {
          fprintf(stderr, "Failed open: test file <%s>\n", options.test);
          exit(EXIT_FAILURE);
  }

  for (i = 0; i < MAX_N; i++) precs[i] = total [i] = 0;

  while (fgets(line, MAXLINE, fd_test) != NULL) {
        line[strlen(line) - 1] = '\0';
        if ((middle = strstr(line, options.separator)) != NULL) {
		strncpy(source, line, middle - line -1);
                source[middle - line -1] = '\0';
                strcpy(reference, middle+strlen(options.separator)+1);
        }
        else {
                strcpy(source, line);
                *reference = '\0';
        }
	next = line;
        for(n = 1; next != middle; n++) {
                next = strchr(next, ' ');
                if (next != NULL) {
                        *next = '\0';
                        next++;
                }
        }
        sentence1 = (char **) malloc(n*sizeof(char *));
        next = line;
        for (i = 1; i < n; i++) {
                sentence1[i] = strdup(next);
                next += strlen(next) + 1;
        }

	if (a != NULL) {
		for (i = 0; i < options.ngram-1; i++) options.l[i] = lambda[i+1];
		options.norm = lambda[options.ngram];
	}
	else for (i = 0; i < options.ngram; i++) options.l[i] = lambda[i+1];
	output = parse(t, UNK, sentence1, words, n, lea, &options, NULL, NULL, NULL, a);
	get_output(target, t->alfa_out, output, candidate);
	// result += bleu(candidate, [reference|test]);
	for (i = 1; i < n; i++) free(sentence1[i]);
        free(sentence1);

	/* if (!strcmp(reference, candidate))
                printf(" 1:  %s %s %s\n", source, options.separator, reference);
        else printf("-1:  %s %s %s %s %s\n", source, options.separator, reference, options.separator, candidate); */

	next = reference;
        for(n1 = 1; next != NULL; n1++) {
                next = strchr(next, ' ');
                if (next != NULL) {
                        *next = '\0';
                        next++;
                }
        }
        sentence1 = (char **) malloc(n1*sizeof(char *));
        next = reference;
        for (i = 1; i < n1; i++) {
                sentence1[i] = strdup(next);
                next += strlen(next) + 1;
        }

	next = candidate;
        for(n2 = 1; next != NULL; n2++) {
                next = strchr(next, ' ');
                if (next != NULL) {
                        *next = '\0';
                        next++;
                }
        }
        sentence2 = (char **) malloc(n2*sizeof(char *));
        next = candidate;
        for (i = 1; i < n2; i++) {
                sentence2[i] = strdup(next);
                next += strlen(next) + 1;
        }

	for (i = 1; i <= MAX_N; i++) {
		prec_sent = prec_n(sentence1, n1, sentence2, n2, i, &total_sent);
		precs[i - 1] += prec_sent;
		total[i - 1] += total_sent;
	}
	refWords += n1;
	sysWords += n2;

	free_output(output);
	for (i = 1; i < n1; i++) free(sentence1[i]);
        free(sentence1);
	for (i = 1; i < n2; i++) free(sentence2[i]);
        free(sentence2);
  }

  //for(i=0;i<MAX_N;i++) fprintf(stderr, "%d: %d/%d\n", i+1, precs[i], total[i]);

  result = 0;
  for(i = 1; i <= MAX_N; i++)
	result += ((float) 1/MAX_N) * my_log((float) precs[i-1] / total[i-1]);
  if (sysWords < refWords) bp = exp(1 - (float) refWords/sysWords);
  else bp = 1;
  result = bp * exp(result);

  fclose(fd_test);

  return 1 - result;
}

float amotry(float **p, float y[], float psum[], int ndim,
	     //float (*funk) (float []), int ihi, float fac) {
	     int ihi, float fac) {
	int j;
	float fac1, fac2, ytry, *ptry;

	ptry = vector(1, ndim);
	fac1 = (1.0 - fac) / ndim;
	fac2 = fac1 - fac;
	for (j = 1; j <= ndim; j++)
		ptry[j] = psum[j]*fac1 - p[ihi][j]*fac2;
	//ytry = (*funk) (ptry);
	ytry = funk(ptry);
	if (ytry < y[ihi]) {
		y[ihi] = ytry;
		for (j = 1; j <= ndim; j++) {
			psum[j] += ptry[j] - p[ihi][j];
			p[ihi][j] = ptry[j];
		}
	}
	free_vector(ptry, 1, ndim);
	return ytry;
}

void amoeba(float **p, float y[], int ndim, float ftol,
	    //float (*funk)(float []), int *nfunk) {
	    int *nfunk) {
	//float amotry(float **p, float y[], float psum[], int ndim,
		     //float (*funk) (float []), int ihi, float fac);
	int i, ihi, ilo, inhi, j, mpts = ndim + 1;
	float rtol, sum, swap, ysave, ytry, *psum;

	psum = vector(1, ndim);
	*nfunk = 0;
	GET_PSUM;
	for (;;) {
		ilo = 1;
		ihi = y[1]>y[2] ? (inhi=2,1) : (inhi=1,2);
		for (i = 1; i <= mpts; i++) {
			if (y[i] <= y[ilo]) ilo = i;
			if (y[i] > y[ihi]) {
				inhi = ihi;
				ihi = i;
			} else if ((y[i] > y[inhi]) && (i != ihi)) inhi = i;
		}
		rtol = 2.0 * fabs(y[ihi] - y[ilo]) / (fabs(y[ihi]) + fabs(y[ilo]) + TINY);
		fprintf(stderr, "Iteration #%d\n", *nfunk);
		for (j = 1; j <= ndim; j++) fprintf(stderr, "%f ", p[ilo][j]);
  		fprintf(stderr, "::: BLEU = %f\n", 1-y[ilo]);
		for (j = 1; j <= ndim; j++) fprintf(stderr, "%f ", p[inhi][j]);
  		fprintf(stderr, "::: BLEU = %f\n", 1-y[inhi]);
		for (j = 1; j <= ndim; j++) fprintf(stderr, "%f ", p[ihi][j]);
  		fprintf(stderr, "::: BLEU = %f\n", 1-y[ihi]);
		if (rtol < ftol) {
			SWAP(y[1],y[ilo])
			for (i = 1; i <= ndim; i++) SWAP(p[1][i],p[ilo][i])
			break;
		}
		if (*nfunk >= NMAX) nrerror("NMAX exceeded");
		*nfunk += 2;
		//ytry = amotry(p, y, psum, ndim, funk, ihi, -1.0);
		ytry = amotry(p, y, psum, ndim, ihi, -1.0);
		if (ytry <= y[ilo])
			//ytry = amotry(p, y, psum, ndim, funk, ihi, 2.0);
			ytry = amotry(p, y, psum, ndim, ihi, 2.0);
		else if (ytry >= y[inhi]) {
			ysave = y[ihi];
			//ytry = amotry(p, y, psum, ndim, funk, ihi, 0.5);
			ytry = amotry(p, y, psum, ndim, ihi, 0.5);
			if (ytry >= ysave) {
				for (i = 1; i <= mpts; i++) {
					if (i != ilo) {
						for (j = 1; j <= ndim; j++)
						   p[i][j] = psum[j] = 0.5*(p[i][j]+p[ilo][j]);
						//y[i] = (*funk) (psum);
						y[i] = funk(psum);
					}
				}
				*nfunk += ndim;
				GET_PSUM;
			}
		}
		else --(*nfunk);
	}
	free_vector(psum, 1, ndim);
}

struct options usage(int argc, char *argv[]) {
  op_parser_t *parser;
  struct options opt;

  parser = op_create("%p -v <transducer file> -t <test file>", PACKAGE_STRING, 1);
  if (parser == NULL) {
          fprintf(stderr, "No s'ha pogut crear el parser d'opcions.\n");
          exit(EXIT_FAILURE);
  }
  op_add_string(parser, 'm', "model", "STRING", "model file without extension", &opt.transducer, NULL);
  op_add_string(parser, 'c', "categ", "FILE", "lema-word file", &opt.table, NULL);
  op_add_string(parser, 't', "test", "FILE", "test file", &opt.test, NULL);
  op_add_string(parser, 'w', "words", "FILE", "words test file", &opt.corpus, NULL);
  op_add_string(parser, 'e', "eagles", "FILE", "eagles test file", &opt.eagles, NULL);
  op_add_string(parser, 's', "sep", "STRING", "separator test file", &opt.separator, "#");
  op_add_flag(parser, 'r', "reord", "Show input/output parsing", &opt.reord);
  op_add_int(parser, 'H', "hash", "N", "Hash table size", &opt.hts, HASH_SIZE);
  op_add_int(parser, 'f', "format", "N", "1 => v2\n2 => arpa", &opt.type, v2);
  op_add_int(parser, 'b', "beam", "N", "Beam-search parameter", &opt.beam, 50);
  op_add_flag(parser, 'p', "smooth", "Backoff smoothing always", &opt.algorithm);
  op_add_string(parser, 'C', "control", "STRING", "control characters", &opt.prefix, NULL);
  op_add_int(parser, 'F', "filter", "N", "Filter model from test", &opt.simbextmax, 0);
  //op_add_flag(parser, 'N', "norm",    "Become an acceptor model", &opt.norm);
  op_add_int(parser, 'n', "nprobs", "N", "Number of probabilities", &opt.ngram, 1);
  op_add_float(parser, 'g', "lambda1", "N", "Weight of the model 1: p(f,e)", &opt.l[0], 1);
  op_add_float(parser, 'i', "lambda2", "N", "Weight of the model 2: p(f|e)", &opt.l[1], 1);
  op_add_float(parser, 'd', "lambda3", "N", "Weight of the model 3: p(e|f)", &opt.l[2], 1);
  op_add_float(parser, 'l', "lambda4", "N", "Weight of the model 4: Word bonus", &opt.l[3], 1);
  op_add_float(parser, 'x', "lambda5", "N", "Weight of the model 5: Exponential", &opt.l[4], 1);
  op_add_float(parser, 'y', "lambda6", "N", "Weight of the model 6: Word penalty", &opt.l[5], 1);
  op_add_float(parser, 'j', "lambda0", "N", "Weight of the model 0: Language model", &opt.norm, 1);
  op_add_string(parser, 'a', "model", "STRING", "model file without extension", &opt.automata, NULL);

  op_parse(parser, argc, argv);

  if ((opt.transducer == NULL) || (opt.test == NULL)) {
        op_usage(parser, stderr);
        exit(EXIT_FAILURE);
  }

  op_destroy(parser);
  return opt;
}

int main(int argc, char *argv[]) {
  int i, j, n_iter = 0;
  float **p, *y, ftol;

  FILE *fd_eagles, *fd_corpus;
  char eagles[MAXLINE], corpus[MAXLINE];
  h_t *table = NULL;
  h_t *vocab = NULL;
  time_t clock;
  char model[MAXLINE], lm[MAXLINE];
  h_t *tableNORM = NULL;
  h_t *d = NULL;

  options = usage(argc, argv);
  for (i = options.ngram+1; i <= NDIM; i++) options.l[i] = 0;
  sprintf(model, "%s.tr", options.transducer);
  t = read_fsm(model, options.hts);
  UNK = hs("<unk>", 6, t->table_out);
  if (options.automata != NULL) {
	sprintf(lm, "%s.tr", options.automata);
	a = read_fsm(lm, options.hts);
        options.ngram++;
  }
  lea = reserva_memoria_trellis(options.beam);

  p = (float **) malloc((options.ngram+2)*sizeof(float *));
  y = (float * ) malloc((options.ngram+2)*sizeof(float  ));
  for (i = 0; i <= options.ngram; i++) p[i+1] = (float *) malloc((options.ngram+1)*sizeof(float));
  for (i = 1; i <= options.ngram; i++)
    for (j = 1; j <= options.ngram; j++)
      if (i == j) p[i][j] = 2.0/(options.ngram+1);
      else p[i][j] = 1.0/(options.ngram+1); 
  for (j = 1; j <= options.ngram; j++) p[options.ngram+1][j] = 1.0/options.ngram;
  /* p[1][1] = p[2][2] = p[3][3] = 0.5;
  p[1][2] = p[1][3] = p[2][1] = p[2][3] = p[3][1] = p[3][2] = 0.25;
  p[4][1] = p[4][2] = p[4][3] = 1.0/3; */

  for (i = 0; i <= options.ngram; i++) {
	y[i+1] = funk(p[i+1]);
	for (j = 1; j <= options.ngram; j++) fprintf(stderr, "%f ", p[i+1][j]);
	fprintf(stderr, "::: BLEU = %f\n", 1-y[i+1]);
  }

  /* ¿Rellenar p con numeros aleatorios? ¿solo positivos?
     Obtener y como y = funk(p);
     ftol = epsilon pequeño ¿TINY? ¿quizá demasiado pequeño? */

  //printf("Entrenamiento de los pesos:\n", n_iter);
  //fprintf(stderr, "(%f, %f, %f) BLEU = %f\n", options.l[0], options.l[1], options.l[2], funk(options.l));;

  amoeba(p, y, options.ngram, 0.001, &n_iter);
  fprintf(stderr, "\n>>> Se han producido %d iteraciones\n\n", n_iter);

  /* Tras n_iter iteraciones, tenemos
     4 resultados: printf (p_0, y_0) (p_1, y_1) (p_2 y_2) (p_3 y_3)
     con y_i pertenece MIN +- ftol */

  for (i = 0; i <= options.ngram; i++) {
    for (j = 1; j <= options.ngram; j++) fprintf(stderr, "%f ", p[i+1][j]);
    fprintf(stderr, "::: BLEU = %f\n", 1-y[i+1]);
  }

  free_trellis(lea);
  free_fsm(t);

}
