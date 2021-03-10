/* 
   giati - Machine Translation via Finite State Models

   Copyright (C) 2006 Jorge González

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/

/* Usage: giati -c <config file> {[-OPTION [param]]} */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "op.h"
#include "fsm.h"
#include "arpa.h"
/* #include "giati.h" */

int GetNextTag(char *line, char *parameters) {
  char tag[MAXCONF];
  int i, length;

  length=strlen(line);
  line[length-1]='\0';
  /* if ((line[0]=='[') && (line[length-2]==']')) {
  	//if (!strcmp(line, "[FITXERS]")) *files = 1;
  	//else *files = 0;
	return CLASS_TAG;
  } */

  if (strchr(line, '=')) {
  	sscanf( (const char *) line, "%[^= ] = %[^=]", tag, parameters);
	if (!strcmp(tag, "WORKDIR")) 		i = 1;
	else if (!strcmp(tag, "SRILMDIR")) 	i = 2;
	else if (!strcmp(tag, "CORPUSDIR")) 	i = 3;
	else if (!strcmp(tag, "TMPDIR")) 	i = 4;
	else if (!strcmp(tag, "CORPUS")) 	i = 5;
	else if (!strcmp(tag, "ALIGN")) 	i = 6;
	else if (!strcmp(tag, "SEPARATOR")) 	i = 7;
	else if (!strcmp(tag, "PREFIX")) 	i = 8;
	else if (!strcmp(tag, "SIMBEXTMAX")) 	i = 9;
	else if (!strcmp(tag, "ALGORITHM")) 	i = 10;
	else if (!strcmp(tag, "NGRAM")) 	i = 11;
	else if (!strcmp(tag, "HASH")) 		i = 12;
	else if (!strcmp(tag, "LEMMAS")) 	i = 13;
	else if (!strcmp(tag, "EAGLES")) 	i = 14;
	else if (!strcmp(tag, "TEST")) 		i = 15;
	else if (!strcmp(tag, "OUTPUT"))	i = 16;
	else i = ERROR_TAG;
	return i;
  }
  else return ERROR_TAG;
}

struct options usage(int argc, char *argv[]) {
  op_parser_t *parser;
  char *prefix, *file, *output;
  FILE *fd_config;
  char line[MAXCONF], parameters[MAXCONF];
  int tag;
  struct options opt;

  parser = op_create("%p -c <configuration file>", PACKAGE_STRING, 1);
  if (parser == NULL) {
  	fprintf(stderr, "No s'ha pogut crear el parser d'opcions.\n");
        exit(EXIT_FAILURE);
  }

  op_add_string(parser, 'c', "conf", "FILE", "configuration file", &file, "./giati.cfg");

  op_parse(parser, argc, argv);

  if ((fd_config = fopen(file, "r")) == NULL ) {
	fprintf(stderr, "Failed open: configuration file <%s>\n", file);
	exit(EXIT_FAILURE);
  }
  while (fgets(line, MAXCONF, fd_config) != NULL) {
	tag = GetNextTag(line, parameters);
	//if (files) {
		switch (tag) {
			case 1: opt.work_dir = strdup(parameters); break;
			case 2: opt.srilm_dir = strdup(parameters); break;
			case 3: opt.corpus_dir = strdup(parameters); break;
			case 4: opt.tmp_dir = strdup(parameters); break;
			case 5: opt.corpus = strdup(parameters); break;
		       case 13: opt.lemmas = strdup(parameters); break;
		       case 14: opt.eagles = strdup(parameters); break;
		       case 15: opt.test = strdup(parameters); break;
			case 6: opt.align = strdup(parameters); break;
			case 7: opt.separator = strdup(parameters); break;
			case 8: opt.prefix = strdup(parameters); break;
		/*}
	}
	else {
		switch (tag) { */
			case 9:  opt.simbextmax = atoi(parameters); break;
			case 10: opt.algorithm = atoi(parameters); break;
			case 11: opt.ngram = atoi(parameters); break;
			case 12: opt.hts = atoi(parameters); break;
			case 16: opt.transducer = strdup(parameters); break;
		}
	//}
  }
  
  fclose(fd_config);

  op_add_int(parser, 'n', "ngram", "N", "n-gram order", &opt.ngram, opt.ngram);
  op_add_string(parser, 'p', "prefix", "STRING", "output prefix", &prefix, opt.prefix);
  op_add_flag(parser, 's', "segment", "Using provided extended symbols file", &opt.reord);
  op_add_int(parser, 'F', "filter", "N", "Filter model from test", &opt.type, 0);
  op_add_flag(parser, 'e', "end",     "Show only the separator characters", &opt.beam);
  op_add_string(parser, 'o', "output", "[arpa|v2]", "Model format", &output, opt.transducer);
  op_add_float(parser, 'g', "lambda1", "N", "Weight of the model 1: p(f,e)", &opt.l[0], 1);
  op_add_float(parser, 'i', "lambda2", "N", "Weight of the model 2: p(f|e)", &opt.l[1], 1);
  op_add_float(parser, 'd', "lambda3", "N", "Weight of the model 3: p(e|f)", &opt.l[2], 1);
  op_add_float(parser, 'd', "lambda3", "N", "Weight of the model 3: p(e|f)", &opt.l[2], 1);
  op_add_float(parser, 'l', "lambda4", "N", "Weight of the model 4: Word bonus", &opt.l[3], 1);
  op_add_float(parser, 'x', "lambda5", "N", "Weight of the model 5: Exponential", &opt.l[4], 1);
  op_add_float(parser, 'y', "lambda6", "N", "Weight of the model 6: Word penalty", &opt.l[5], 1);
  op_add_float(parser, 'j', "lambda0", "N", "Weight of the model 0: Language model", &opt.norm, 1);
  op_add_string(parser, 'a', "model", "STRING", "model file without extension", &opt.automata, NULL);

  op_parse(parser, argc, argv);

  if (prefix != opt.prefix) strcpy(opt.prefix, prefix);
  if (output != opt.transducer) strcpy(opt.transducer, output);

  op_destroy(parser);
  return opt;
}

void write_parameters(struct options *opt) {
  char params[MAXCONF];
  FILE *fd_params;

  sprintf(params, "%s/%s%s.params", opt->corpus_dir, opt->prefix, opt->corpus);
  if ((fd_params = fopen(params, "w")) == NULL ) {
	fprintf(stderr, "Failed write: parameters file <%s>\n", params);
        exit(EXIT_FAILURE);
  }
  fprintf(fd_params, "WORKDIR %s\n", opt->work_dir);
  fprintf(fd_params, "SRILMDIR %s\n", opt->srilm_dir);
  fprintf(fd_params, "CORPUSDIR %s\n", opt->corpus_dir);
  fprintf(fd_params, "TMPDIR %s\n", opt->tmp_dir);
  fprintf(fd_params, "CORPUS %s\n", opt->corpus);
  fprintf(fd_params, "LEMMAS %s\n", opt->lemmas);
  fprintf(fd_params, "EAGLES %s\n", opt->eagles);
  fprintf(fd_params, "TEST %s\n", opt->test);
  fprintf(fd_params, "ALIGN %s\n", opt->align);
  fprintf(fd_params, "SEPARATOR %s\n", opt->separator);
  fprintf(fd_params, "PREFIX %s\n", opt->prefix);
  fprintf(fd_params, "SIMBEXTMAX %d\n", opt->simbextmax);
  fprintf(fd_params, "ALGORITHM %d\n", opt->algorithm);
  fprintf(fd_params, "NGRAM %d\n", opt->ngram);
  fprintf(fd_params, "OUTPUT %s\n", opt->transducer);
  fprintf(fd_params, "HASH %d\n", opt->hts);
  fclose(fd_params);
}

void freedom(struct options *opt) {

  free(opt->work_dir);
  free(opt->srilm_dir);
  free(opt->corpus_dir);
  free(opt->tmp_dir);
  free(opt->corpus);
  free(opt->lemmas);
  free(opt->eagles);
  free(opt->test);
  free(opt->align);
  free(opt->separator);
  free(opt->prefix);
  free(opt->transducer);
}

int main(int argc, char *argv[]) {
  struct options options;
  unsigned char c1, c2, c3, c4;
  h_t *table;
  T_REDGEN *v2;
  char train[MAXCONF], test[MAXCONF], fsm[MAXCONF];

  options = usage(argc, argv);
  write_parameters(&options);
  
  search_characters(&options, &c1, &c2, &c3, &c4);
  if (options.beam) {
	  printf("%c(%d) %c(%d)\n", c1, c1, c2, c2);
  	  freedom(&options);
	  exit(1);
  }
  if (!options.reord)
	  write_extended_simbols(&options, c1, c2);

  get_coded_corpus(&options);
  get_automaton(&options);

  if (!strcmp(options.transducer, "v2")) {
	sprintf(train, "%s/%s%s", options.corpus_dir, options.prefix, options.corpus);
	sprintf(test, "%s/%s", options.corpus_dir, options.test);
  	v2 = arpa2fsm(train, test,
		      options.type, options.hts, options.separator, options.algorithm, c1, c2, Training, &options);
	sprintf(fsm, "%s.tr", train);
	write_fsm(v2, fsm, 1, &options);
	free_fsm(v2);
  }

  if ((options.algorithm == 2) || (options.algorithm == 4)) get_l2w_table(&options, c1);

  freedom(&options);

}
