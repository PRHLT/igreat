typedef struct {
	//int origen;
	int estado;
	double score;
	int input;
	l_int *output;
	int lm_estado;
} ea;

ea **reserva_memoria_trellis(int size);
l_int *parse(T_REDGEN *A, int *UNK, char **source, char *corpus, int n, ea **lea, struct options *opt, char *eagles, h_t *table, h_t *vocab, T_REDGEN *B);
