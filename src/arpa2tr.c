#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "op.h"
#include "fsm.h"
#include "arpa.h"
#include "refx.h"
#include "l2w.h"
#include "parse.h"

struct options usage(int argc, char *argv[]) {
  op_parser_t *parser;
  struct options opt;

  parser = op_create("$ %p -m <arpa file> -C 'XY'\n       X and Y are the control characters", PACKAGE_STRING, 1);
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
  op_add_int(parser, 'f', "format", "N", "1 => tr\n2 => lm", &opt.type, v2);
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

  if ((opt.transducer == NULL) || (opt.prefix == NULL)) {
	op_usage(parser, stderr);
	exit(EXIT_FAILURE);
  }

  op_destroy(parser);
  return opt;
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
  char model[MAXLINE];
  h_t *tableNORM = NULL;
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

  c1 = opt->prefix[0];
  c2 = opt->prefix[1];

  clock = time(NULL);
  fprintf(stderr, "Reading ARPA language model... %s", asctime(localtime(&clock)));

  t = arpa2fsm(opt->transducer, opt->test,
	       opt->simbextmax, opt->hts, opt->separator, opt->reord + 1, c1, c2, Training, opt);
	  
  write_fsm(t, model, opt->ngram, opt);
  free_fsm(t);

  clock = time(NULL);
  fprintf(stderr, "...done %s", asctime(localtime(&clock)));
}


int main(int argc, char *argv[]) {
  struct options options;

  options = usage(argc, argv);

  refx(&options);

}
