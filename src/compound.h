#define MAX_PHRASE_LENGTH 7
typedef struct {
     int start, stop;
} segment;

typedef struct _sa {
     int j, i;
     struct _sa *ant;
     struct _sa *sig;
} sa;

void search_characters(struct options *opt, char *c1, char *c2,
                                            char *c3, char *c4);

void build_compound_sentence(char *source, int sourcel, char *target, int targetl, char separator,
			     int *alignments, int algorithm, int simbextmax, char *output);

void write_extended_simbols(struct options *opt, char c1, char c2);
