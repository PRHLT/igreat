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

  parser = op_create("%p -v <transducer file> -t <test file>", PACKAGE_STRING, 1);
  if (parser == NULL) {
          fprintf(stderr, "No s'ha pogut crear el parser d'opcions.\n");
          exit(EXIT_FAILURE);
  }
  op_add_string(parser, 'm', "model", "STRING", "model file without extension", &opt.transducer, NULL);
  op_add_string(parser, 't', "test", "FILE", "test file", &opt.test, NULL);
  op_add_string(parser, 's', "sep", "STRING", "separator test file", &opt.separator, "#");

  op_add_int(parser, 'b', "beam", "N", "Beam-search parameter", &opt.beam, 50);
  op_add_flag(parser, 'p', "smooth", "Backoff smoothing always", &opt.algorithm);

  op_add_int(parser, 'f', "format", "N", "1 => v2\n2 => arpa", &opt.type, v2);
  op_add_string(parser, 'C', "control", "STRING", "control characters", &opt.prefix, NULL);
  op_add_int(parser, 'F', "filter", "N", "Filter model from test", &opt.simbextmax, 0);

  op_add_int(parser, 'n', "nprobs", "N", "Number of probabilities", &opt.ngram, 1);
  op_add_float(parser, 'g', "lambda1", "N", "Weight of the model 1: p(f,e)", &opt.l[0], 1);
  op_add_float(parser, 'i', "lambda2", "N", "Weight of the model 2: p(f|e)", &opt.l[1], 1);
  op_add_float(parser, 'd', "lambda3", "N", "Weight of the model 3: p(e|f)", &opt.l[2], 1);
  op_add_float(parser, 'l', "lambda4", "N", "Weight of the model 4: Word bonus", &opt.l[3], 1);
  op_add_float(parser, 'x', "lambda5", "N", "Weight of the model 5: Exponential", &opt.l[4], 1);
  op_add_float(parser, 'y', "lambda6", "N", "Weight of the model 6: Word penalty", &opt.l[5], 1);
  op_add_float(parser, 'j', "lambda0", "N", "Weight of the model 0: Language model", &opt.norm, 1);
  op_add_string(parser, 'a', "model", "STRING", "model file without extension", &opt.automata, NULL);

  op_add_string(parser, 'c', "categ", "FILE", "lema-word file", &opt.table, NULL);
  op_add_string(parser, 'w', "words", "FILE", "words test file", &opt.corpus, NULL);
  op_add_string(parser, 'e', "eagles", "FILE", "eagles test file", &opt.eagles, NULL);
  op_add_flag(parser, 'r', "reord", "Show input/output parsing", &opt.reord);
  op_add_int(parser, 'H', "hash", "N", "Hash table size", &opt.hts, HASH_SIZE);

  op_parse(parser, argc, argv);

  if ((opt.transducer == NULL) || (opt.test == NULL)) {
	op_usage(parser, stderr);
	exit(EXIT_FAILURE);
  }

  op_destroy(parser);
  return opt;
}

void refx(struct options *opt) {
  T_REDGEN *t = NULL, *a = NULL;
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
  char model[MAXLINE], lm[MAXLINE];
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

  if (opt->type == arpa) {
	  //fprintf(stderr, "%s\n", opt->prefix);
	  c1 = opt->prefix[0];
	  c2 = opt->prefix[1];

  	  clock = time(NULL);
  	  fprintf(stderr, "Reading ARPA language model...... %s", asctime(localtime(&clock)));

	  t = arpa2fsm(opt->transducer, opt->test,
		       //opt->simbextmax, opt->hts, opt->separator, opt->reord + 1, c1, c2, Decoding, opt->norm);
		       opt->simbextmax, opt->hts, opt->separator, opt->reord + 1, c1, c2, Training, opt);
	  
	  //write_fsm(t, model, opt->ngram, opt);
	  //free_fsm(t);

  	  clock = time(NULL);
  	  fprintf(stderr, "...done %s", asctime(localtime(&clock)));

	  //if (d != NULL) hd(d, 1);
	  //exit(1);
  }
  else {
  	clock = time(NULL);
  	fprintf(stderr, "Reading SFST translation model... %s", asctime(localtime(&clock)));

  	t = read_fsm(model, opt->hts);

  	clock = time(NULL);
  	fprintf(stderr, "...done %s", asctime(localtime(&clock)));

	/* write_fsm(t, "/home/jgonza/kk.tr", opt->ngram);
	free_fsm(t);
	exit(1); */
  }

  if (opt->reord) {
	  alfa = read_table(opt, &table, &vocab);
  	  if ((fd_eagles = fopen(opt->eagles, "r")) == NULL ) {
	  	fprintf(stderr, "Failed open: eagles test file <%s>\n", opt->eagles);
	  	exit(EXIT_FAILURE);
	  }
  	  if ((fd_corpus = fopen(opt->corpus, "r")) == NULL ) {
	  	fprintf(stderr, "Failed open: words test file <%s>\n", opt->corpus);
	  	exit(EXIT_FAILURE);
	  }
  }

  //if (opt->norm) tableNORM = hc(opt->hts);

  UNK = hs("<unk>", 6, t->table_out);

  if (opt->automata != NULL) {
  	sprintf(lm, "%s.tr", opt->automata);
  	clock = time(NULL);
  	fprintf(stderr, "Reading SFST language model... %s", asctime(localtime(&clock)));

  	a = read_fsm(lm, opt->hts);

  	clock = time(NULL);
  	fprintf(stderr, "...done %s", asctime(localtime(&clock)));
  }

  if ((fd_test = fopen(opt->test, "r")) == NULL ) {
	  fprintf(stderr, "Failed open: test file <%s>\n", opt->test);
	  exit(EXIT_FAILURE);
  }

  lea = reserva_memoria_trellis(opt->beam);

  clock = time(NULL);
  fprintf(stderr, "\nTranslating... %s", asctime(localtime(&clock)));

  while (fgets(line, MAXLINE, fd_test) != NULL) {
	  line[strlen(line) - 1] = '\0';
	  if ((middle = strstr(line, opt->separator)) != NULL) {
		 strncpy(source, line, middle - line -1);
		 source[middle - line -1] = '\0';
		 strcpy(reference, middle+strlen(opt->separator)+1);
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

	  sentence = (char **) malloc(n*sizeof(char *));
	  next = line;
	  for (i = 1; i < n; i++) {
		  sentence[i] = strdup(next);
		  next += strlen(next) + 1;
	  }

	  if (opt->reord == 1) {
		  fgets(eagles, MAXLINE, fd_eagles);
		  fgets(words, MAXLINE, fd_corpus);
		  words[strlen(words) - 1] = '\0';
		  if ((middle = strstr(words, opt->separator)) != NULL) strcpy(reference, middle+4);
		  else *reference = '\0';
	  }

  	  /* clock = time(NULL);
  	  fprintf(stderr, "\nANTES Parsing... %s", asctime(localtime(&clock)));
  	  fflush(stderr); */

	  output = parse(t, UNK, sentence, words, n, lea, opt, eagles, table, vocab, a);

  	  /* clock = time(NULL);
  	  fprintf(stderr, "\nDESPUES Parsing... %s", asctime(localtime(&clock)));
  	  fflush(stderr); */

	  if (opt->reord) target = alfa;
	  get_output(target, t->alfa_out, output, candidate);

	  /* clock = time(NULL);
  	  fprintf(stderr, "\nDESPUES Parsing... (y 2) %s", asctime(localtime(&clock)));
  	  fflush(stderr); */
	  
	  //else target = words;
	  //printf("\n\n%%%%%%%%%   TRADUCCION FINAL = \"");
	  //get_output(alfa, t->alfa_out, output, candidate);
	  //fflush(stdout);

	  if (!strcmp(reference, candidate))
		  printf(" 1:  %s %s %s\n", source, opt->separator, reference);
	  else printf("-1:  %s %s %s %s %s\n", source, opt->separator, reference, opt->separator, candidate);

	  /* clock = time(NULL);
  	  fprintf(stderr, "\nDESPUES Parsing... (y 3) %s", asctime(localtime(&clock)));
  	  fflush(stderr); */
	  
	  //for (i = 1; i <= n; i++) free(words[i]);
	  free_output(output);

	  for (i = 1; i < n; i++) free(sentence[i]);
	  free(sentence);

	  /* clock = time(NULL);
  	  fprintf(stderr, "\nDESPUES Parsing... (y 4) %s", asctime(localtime(&clock)));
  	  fflush(stderr); */
  }

  fclose(fd_test);
  fflush(stdout);
  clock = time(NULL);
  fprintf(stderr, "...done %s", asctime(localtime(&clock)));
  fflush(stderr);

  /* clock = time(NULL);
  fprintf(stderr, "\nDESPUES Parsing... (y 5) %s", asctime(localtime(&clock)));
  fflush(stderr); */

  if (t != NULL) {
	  //write_fsm(t, "/home/jgonza/kk");
	  free_trellis(lea);

  	  clock = time(NULL);
  	  fprintf(stderr, "\nFreeing memory... %s", asctime(localtime(&clock)));

	  free_fsm(t);

  	  fprintf(stderr, "\n...done %s", asctime(localtime(&clock)));
  }
  if (opt->reord == 1) {
	  fclose(fd_corpus);
	  fclose(fd_eagles);
	  hd(vocab, 1);
	  hd(table, 0);
	  free(alfa);
  }
  //if (opt->norm) hd(tableNORM, 1);
}


int main(int argc, char *argv[]) {
  struct options options;

  options = usage(argc, argv);

  refx(&options);

}
