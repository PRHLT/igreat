/***************************************************************************
                          hash.c  -  description
                             -------------------
    begin                : dom nov 21 2004
    copyright            : (C) 2004 by Jorge Civera Saiz
    email                : jorcisai@iti.upv.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define _GNU_SOURCE 
#include <stdlib.h>
#include <string.h>

#include "auxiliar.h"
#include "hash.h"

/***************************************************************************/
/***************************************************************************/

/* the key */ /* key length */ /* the previous hash, or an arbitrary value */    
ub4 hf( k, length, initval)
register ub1 *k;        /* the key */
register ub4  length;   /* the length of the key */
register ub4  initval;  /* the previous hash, or an arbitrary value */
{
   register ub4 a,b,c,len;
   
   /* Set up the internal state */
   len = length;
   a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
   c = initval;         /* the previous hash value */
   
   /*---------------------------------------- handle most of the key */
   while (len >= 12)
   {
      a += (k[0] +((ub4)k[1]<<8) +((ub4)k[2]<<16) +((ub4)k[3]<<24));
      b += (k[4] +((ub4)k[5]<<8) +((ub4)k[6]<<16) +((ub4)k[7]<<24));
      c += (k[8] +((ub4)k[9]<<8) +((ub4)k[10]<<16)+((ub4)k[11]<<24));
      mix(a,b,c);
      k += 12; len -= 12;
   }
  
   /*------------------------------------- handle the last 11 bytes */
   c += length;
   switch(len)              /* all the case statements fall through */
   {
   case 11: c+=((ub4)k[10]<<24);
   case 10: c+=((ub4)k[9]<<16);
   case 9 : c+=((ub4)k[8]<<8);
      /* the first byte of c is reserved for the length */
   case 8 : b+=((ub4)k[7]<<24);
   case 7 : b+=((ub4)k[6]<<16);
   case 6 : b+=((ub4)k[5]<<8);
   case 5 : b+=k[4];
   case 4 : a+=((ub4)k[3]<<24);
   case 3 : a+=((ub4)k[2]<<16);
   case 2 : a+=((ub4)k[1]<<8);
   case 1 : a+=k[0];
     /* case 0: nothing left to add */
   }
   mix(a,b,c);   
   /*-------------------------------------------- report the result */
   return c;
} 

/***************************************************************************/

h_t *hc(int hsize){

  int i;
  h_t *ht;

  memtest(ht=(h_t *)malloc(sizeof(h_t)));
  ht->hsize=hsize;
  memtest(ht->htable=(he_t **)malloc(hsize*sizeof(he_t *)));
  for(i=0;i<hsize;i++) ht->htable[i]=NULL;
  for(i=0;hsize!=0;i++) hsize=hsize>>1;
  ht->ubits=i-1;
  return ht;
}

/***************************************************************************/
/***************************************************************************/

ub4 hash(char *key, int key_length, int ubits){
  return hf(key, key_length, INITVAL) & HASHMASK(ubits);
}    

/***************************************************************************/
/***************************************************************************/

void *hs(char *key, int key_length, h_t *ht){
  
  he_t *p;

  for (p=ht->htable[hash(key,key_length,ht->ubits)];p!=NULL;p=p->p)
    if (!strncmp(key,p->key,key_length)) break;
  return (p==NULL)?(void *) -1:p->data;
}

/***************************************************************************/
/***************************************************************************/

void *hi(char *key, int key_length, void *data, h_t *ht){

  he_t *p,*pp;
  
  for (pp=NULL,p=ht->htable[hash(key,key_length,ht->ubits)];p!=NULL;pp=p,p=p->p)
    if (!strncmp(key,p->key,key_length)) break;
  /* We have already inserted this element */  
  if (p!=NULL) return p->data;
  /* New element in hash table */
  memtest(p=(he_t *)malloc(sizeof(he_t)));
  p->key=strdup(key); p->data=data;
  /* New entry in hash table */
  if (pp==NULL) {p->p=NULL; ht->htable[hash(key,key_length,ht->ubits)]=p;}
  else {p->p=pp->p; pp->p=p;}
  return data;  
}

/***************************************************************************/
/***************************************************************************/

void hd(h_t *ht, int is_pointer){

  int i;
  he_t *p,*pp;

  for(i=0;i<ht->hsize;i++)
    for (p=ht->htable[i];p!=NULL;pp=p,p=p->p,free(pp),pp=NULL)
      {free(p->key);p->key=NULL;if(is_pointer) {free(p->data);p->data=NULL;}}

  free(ht->htable);ht->htable=NULL;
  free(ht);ht=NULL;
}
