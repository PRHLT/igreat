#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include "op.h"
#include "fsm.h"
#include "arpa.h"
#include "refx.h"
#include "l2w.h"
#include "parse.h"

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

h_t *read_lex(struct options *opt) {
  char model[MAXLINE], source[MAXLINE], target[MAXLINE], key[MAXLINE];
  double p1, p2;
  FILE *fd_input;
  lexicon *l;
  h_t *d;

  sprintf(model, "%s.lex", opt->transducer);
  if ((fd_input = fopen(model, "r")) == NULL )
        return NULL;

  d = hc(opt->hts);

  while (fscanf(fd_input, "%s %s %lf %lf", source, target, &p1, &p2) != EOF) {
	sprintf(key, "%s %s", source, target);
	memtest(l = (lexicon *) malloc(sizeof(lexicon)));
        l->p[1] = p1;
        l->p[2] = p2;
        hi(key, strlen(key)+1, l, d);
  }

  fclose(fd_input);
  /* if ((fd_input = fopen(model, "r")) == NULL ) {
        fprintf(stderr, "Failed open: lexicon file <%s>\n", model);
        exit(EXIT_FAILURE);
  }
  while (fscanf(fd_input, "%s %s %lf %lf", source, target, &p1, &p2) != EOF) {
	sprintf(key, "%s %s", source, target);
	l = hs(key, strlen(key)+1, d);
	printf("%s %g %g\n", key, l->p[1], l->p[2]);
  }
  fclose(fd_input); */
  return d;
}

double Poisson(int I, double Jr) {
  double p;
  int i;

  p = Jr*log10(exp(1)) - I*log10(Jr);
  for (i = 2; i <= I; i++) p+= log10(i);
  
  return p;
}

lexicon *ibm1(char *symbols, char c2, h_t *d, double r) {
  int i, j, I, J;
  char e[MAXWORDS][MAXLINE], f[MAXWORDS][MAXLINE], key[MAXLINE];
  char *p1, *p2;
  lexicon *l, *aux;
  double acum, pef, pfe;

  if (d == NULL) return NULL;

  p1 = symbols;

  strcpy(f[0], "NULL");
  strcpy(e[0], "NULL");
  I = J = 1;
  while ((p2 = strchr(p1, c2)) != NULL) {
        strncpy(f[J], p1, p2 - p1);
        f[J][p2 - p1] = '\0';
        J++;
        p1 = p2 + 1;
  }
  if ((p2 = strchr(p1, ' ')) != NULL) {
        strncpy(f[J], p1, p2 - p1);
        f[J][p2 - p1] = '\0';
        J++;
        p1 = p2 + 1;
  }
  while ((p2 = strchr(p1, ' ')) != NULL) {
        strncpy(e[I], p1, p2 - p1);
        e[I][p2 - p1] = '\0';
        I++;
        p1 = p2 + 1;
  }
  if (J > 1) {
        strcpy(e[I], p1);
        I++;
  }
  else {
        strcpy(f[J], p1);
        J++;
  }

  J--; I--;

  pfe = 0;
  for (j = 1; j <= J; j++) {
        acum = 0;
        for (i = 0; i <= I; i++) {
                sprintf(key, "%s %s", f[j], e[i]);
                if ((aux = hs(key, strlen(key)+1, d)) != (lexicon *) -1)
                        acum = acum + aux->p[1];
        }
        if (acum > 0) {
                acum = -log10(acum);
                pfe = pfe + acum;
        }
        else {
                pfe = INT_MAX;
                break;
        }
  }
  if (pfe != INT_MAX) pfe += Poisson(J, I/r);

  pef = 0;
  for (i = 1; i <= I; i++) {
        acum = 0;
        for (j = 0; j <= J; j++) {
                sprintf(key, "%s %s", f[j], e[i]);
                if ((aux = hs(key, strlen(key)+1, d)) != (lexicon *) -1)
                        acum = acum + aux->p[2];
        }
        if (acum > 0) {
                acum = -log10(acum);
                pef = pef + acum;
        }
        else {
                pef = INT_MAX;
                break;
        }
  }
  if (pef != INT_MAX) pef += Poisson(I, J*r);

  l = (lexicon *) malloc(sizeof(lexicon));
  l->p[1] = pfe;
  l->p[2] = pef;

  return l;
}

double vocabextprobs(char *vocabext, char c1, char c2, h_t *d) {
  FILE *fd_vocabext, *fd_vocabextprobs;
  int i, n, max = 0, trg = 0, total;
  char word[MAXLINE], *p;
  double ratio, avgtrg, avgsrc;
  char vocabextprobs[MAXLINE];
  lexicon *l;

  if ((fd_vocabext = fopen(vocabext, "r")) == NULL ) {
          fprintf(stderr, "Failed open: extended vocabulary file <%s>\n", vocabext);
          exit(EXIT_FAILURE);
  }

  while (fscanf(fd_vocabext, "%d %s", &i, word) != EOF) {
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

          //memtest((*vocabulary)[i] = strdup(word));
  }

  fclose(fd_vocabext);

  total = i;
  avgsrc = ((double) (max + total)) / total;
  avgtrg = (double) trg / total;
  ratio = avgtrg / avgsrc;

  if ((fd_vocabext = fopen(vocabext, "r")) == NULL ) {
          fprintf(stderr, "Failed open: extended vocabulary file <%s>\n", vocabext);
          exit(EXIT_FAILURE);
  }
  sprintf(vocabextprobs, "%s.probs", vocabext);
  if ((fd_vocabextprobs = fopen(vocabextprobs, "w")) == NULL ) {
          fprintf(stderr, "Failed open: weighted extended vocabulary file <%s>\n", vocabextprobs);
          exit(EXIT_FAILURE);
  }
  while (fscanf(fd_vocabext, "%d %s", &i, word) != EOF) {
        p = word;
        trg = 0;
        while ((p = strchr(p, c1)) != NULL) {
	      *p = ' ';
              trg++;
	}
        //printf("Analizando ... %s", word);
  	l = (lexicon *) ibm1(word, c2, d, ratio);
        //printf("... %g %g\n", pow(10, -l->p1), pow(10, -l->p2));
        p = word;
        while ((p = strchr(p, ' ')) != NULL) *p = c1;
        fprintf(fd_vocabextprobs, "%d %s %g %g %d\n", i, word, pow(10, -l->p[1]), pow(10, -l->p[2]), trg);
        //fprintf(fd_vocabextprobs, "%d %s\n", i, word);
        free(l);
  }
  fclose(fd_vocabext);
  fclose(fd_vocabextprobs);
        
}

void refx(struct options *opt) {
  T_REDGEN *t = NULL;
  FILE *fd_test, *fd_eagles, *fd_corpus;
  char line[MAXLINE], *middle;
  char eagles[MAXLINE], corpus[MAXLINE];
  char source[MAXLINE], reference[MAXLINE], candidate[MAXLINE];
  char words[MAXLINE], **sentence;
  int i, n, n_id;
  ea **lea;
  l_int *output;
  int *UNK;
  h_t *table = NULL;
  h_t *vocab = NULL;
  char **alfa, **target;
  time_t clock;
  char *next, c1, c2;
  char model[MAXLINE], vocabext[MAXLINE];
  //h_t *tableNORM = NULL;
  h_t *d = NULL;
  
  
  /* fprintf(stderr, "Traductor: %s\n", opt->transducer);
  fprintf(stderr, "Tabla    : %s\n", opt->table);
  fprintf(stderr, "Test     : %s\n", opt->test);
  fprintf(stderr, "Eagles   : %s\n", opt->eagles);
  fprintf(stderr, "Separator: %s\n", opt->separator); 
  fprintf(stderr, "Reord: %d\n", opt->reord); 
  fprintf(stderr, "HTS: %d\n", opt->hts); 
  fprintf(stderr, "Type: %d\n", opt->type); 
  fprintf(stderr, "Beam: %d\n", opt->beam); */

  sprintf(model, "%s.tr", opt->transducer);
  sprintf(vocabext, "%s.vocabext", opt->transducer);

  if (opt->type == arpa) {
	  //fprintf(stderr, "%s\n", opt->prefix);
	  c1 = opt->prefix[0];
	  c2 = opt->prefix[1];

  	  clock = time(NULL);
  	  fprintf(stderr, "Reading lexical model... %s", asctime(localtime(&clock)));

	  d = read_lex(opt);

	  if (d != NULL) {
  	  	clock = time(NULL);
  	  	fprintf(stderr, "...done %s", asctime(localtime(&clock)));
	  }

          vocabextprobs(vocabext, c1, c2, d);
  }
}

int main(int argc, char *argv[]) {
  struct options options;

  options = usage(argc, argv);

  refx(&options);

}
