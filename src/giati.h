#define MAXCONF 1000
#define MAXLINE 4000
#define MAXWORDS 101
#define MAXWORDLENGTH 100
#define ERROR_TAG -1
#define CLASS_TAG 0
#define MaxProb 10

struct options {
	char *work_dir;
	char *srilm_dir;
	char *corpus_dir;
	char *tmp_dir;
        
	char *corpus;
	char *lemmas;
	char *eagles;
	char *align;
	char *separator;
	char *prefix;

	int simbextmax;
	int algorithm;
	int ngram;
	int hts;

	char *transducer;
	char *automata;
	char *table;
	char *test;
	int reord;
	int type;
	int beam;

	float norm; 

	float l[MaxProb];
};

typedef struct {
	double p[MaxProb];
	int n;
} lexicon;
