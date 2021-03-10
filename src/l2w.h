typedef struct l2w {
	char *word;
	int frequency;
	struct l2w *next;
} l_wordcount;

l_int *read_entry(l_int *output, char **dict, char **tags, int n, h_t *table, h_t *vocab);
char **read_table(struct options *opt, h_t **table, h_t **vocab);
void get_l2w_table(struct options *opt, char c1);
