/***************************************************************************
                          auxiliar.h  -  description
                             -------------------
    begin                : Tue Aug 26 2003
    copyright            : (C) 2003 by Instituto Tecnologico de Informatica (ITI)
    email                : tt2iti@iti.upv.es
 ***************************************************************************/
#ifndef AUXILIAR_INCLUDED
#define AUXILIAR_INCLUDED

/* Log function employed as LOG */
#define LOG(X) log(X)

typedef struct timeval timeval_t;

/* Check if memory was allocated to pointer ptr */
void memtest(void *ptr);

int levenshtein_distance(char *s,char*t);

double elapsed_time(timeval_t *t1, timeval_t *t2);

#endif
