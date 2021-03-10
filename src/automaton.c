#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "giati.h"
#include "automaton.h"

void get_coded_corpus(struct options *opt) {
  char extended[MAXCONF], cod_extended[MAXCONF], vocabext[MAXCONF];
  FILE *fd_extended, *fd_codextended, *fd_vocabext;
  char line[MAXLINE], word[MAXLINE];
  char *next;
  int id, *index;
  h_t *table;

  sprintf(extended,     "%s/%s%s.compost",     opt->work_dir, opt->prefix, opt->corpus);
  sprintf(cod_extended, "%s/%s%s.compostcod",  opt->tmp_dir,  opt->prefix, opt->corpus);
  sprintf(vocabext,     "%s/%s%s.vocabext",    opt->corpus_dir,  opt->prefix, opt->corpus);

  if ((fd_extended = fopen(extended, "r")) == NULL ) {
         fprintf(stderr, "Failed open: extended file <%s>\n", extended);
         exit(EXIT_FAILURE);
  }

  if ((fd_codextended = fopen(cod_extended, "w")) == NULL ) {
         fprintf(stderr, "Failed write: coded extended file <%s>\n", cod_extended);
         exit(EXIT_FAILURE);
  }

  if ((fd_vocabext = fopen(vocabext, "w")) == NULL ) {
         fprintf(stderr, "Failed write: extended vocabulary file <%s>\n", vocabext);
         exit(EXIT_FAILURE);
  }
  
  id = 1;
  table = hc(opt->hts);
  while (fgets(line, MAXLINE, fd_extended) != NULL) {
	line[strlen(line)-1]='\0';

	next = line;
        while (next != NULL) {
		sscanf( (const char *) next, "%s", word);

		index = hs(word, strlen(word)+1, table);
		if (index != (int *) -1)
			fprintf(fd_codextended, "%d", *index);
		else {
			memtest(index = (int *) malloc(sizeof(int)));
			*index = id;
			hi(word, strlen(word)+1, index, table);
			fprintf(fd_codextended, "%d", id);
			fprintf(fd_vocabext, "%d %s\n", id, word);
			id++;
		}

		next = strchr(next, ' ');
                if (next != NULL) {
			fprintf(fd_codextended, " ");
                	next++;
		}
	}
	fprintf(fd_codextended, "\n");
  }

  fclose(fd_extended);
  fclose(fd_codextended);
  fclose(fd_vocabext); 

  hd(table, 1);

}

void get_automaton(struct options *opt) {
  char cod_extended[MAXCONF], toprune[MAXCONF], arpa[MAXCONF];
  FILE *fd_codextended, *fd_toprune, *fd_arpa;
  char cutoffs[MAXCONF]="-gt3min 4 -gt4min 4 -gt5min 5 -gt6min 4 -gt7min 4 -gt8min 4 -gt9min 4";
  char command[MAXLINE];

  sprintf(cod_extended, "%s/%s%s.compostcod",  opt->tmp_dir,  opt->prefix, opt->corpus);
  sprintf(toprune,      "%s/%s%s.arpanoprune", opt->tmp_dir,  opt->prefix, opt->corpus);
  sprintf(arpa,         "%s/%s%s.arpa",        opt->corpus_dir,  opt->prefix, opt->corpus);
  sprintf(command, "%s/ngram-count -order %d -unk -text %s -lm %s", opt->srilm_dir, opt->ngram, cod_extended, arpa);

  system ((const char *) command);
}
