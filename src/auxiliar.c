/***************************************************************************
                          auxiliar.c  -  description
                             -------------------
    begin                : Tue Aug 26 2003
    copyright            : (C) 2003 by Instituto Tecnologico de Informatica (ITI)
    email                : tt2iti@iti.upv.es
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "auxiliar.h"

/* Check if memory was allocated to pointer ptr */
void memtest(void *ptr)
{
  if (ptr==NULL) {
    fprintf(stderr,"%s: insufficient memory\n",__FILE__);
    exit(EXIT_FAILURE);
  }
}

double elapsed_time(timeval_t *t1, timeval_t *t2){

  long sec;
  long usec;

  sec = (*t2).tv_sec - (*t1).tv_sec;
  if ((*t1).tv_usec <  (*t2).tv_usec)
    usec = (*t2).tv_usec - (*t1).tv_usec;
  else{
    usec = 1000000 - (*t1).tv_usec + (*t2).tv_usec;
    sec--;
  }

  return (double) sec + ((double) usec / 1000000);

}

#ifdef WIN

char* strdup(const char* string){

char* string_copy=NULL;

if(string!=NULL){
  memtest(string_copy=malloc(sizeof(char)*(strlen(string)+1)));
  memcpy(string_copy,string, sizeof(char)*(strlen(string)+1));
}

return string_copy;

}


char* strndup(const char* string, int length){

char* string_copy=NULL;

if(string!=NULL){
  memtest(string_copy=malloc(sizeof(char)*(length+1)));
  memcpy(string_copy,string, sizeof(char)*(length));
  string_copy[length]='\0';
}

return string_copy;

}

#endif

