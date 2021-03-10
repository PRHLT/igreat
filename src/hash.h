/***************************************************************************
                          hash.h  -  description
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

/***************************************************************************/
/*************************CONSTANTS*****************************************/
/***************************************************************************/

#define INITVAL 23312819
#define HASHSIZE(n) ((ub4)1<<(n))
#define HASHMASK(n) (HASHSIZE(n)-1)

/***************************************************************************/
/************************ TYPE DEFINITION **********************************/
/***************************************************************************/
      
typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
typedef  unsigned       char ub1;   /* unsigned 1-byte quantities */

typedef struct he_type{
  char *key;
  void *data;
  struct he_type *p;
} he_t;

typedef struct h_type{
  he_t **htable;
  int hsize;
  int ubits;
} h_t;

/***************************************************************************/
/*******************************FUNCTIONS***********************************/
/***************************************************************************/

/*
--------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.
For every delta with one or two bits set, and the deltas of all three
  high bits or all three low bits, whether the original value of a,b,c
  is almost all zero or is uniformly distributed,
* If mix() is run forward or backward, at least 32 bits in a,b,c
  have at least 1/4 probability of changing.
y* If mix() is run forward, every bit of c will change between 1/3 and
  2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
mix() was built out of 36 single-cycle latency instructions in a
  structure that could supported 2x parallelism, like so:
      a -= b;
      a -= c; x = (c>>13);
      b -= c; a ^= x;
      b -= a; x = (a<<8);
      c -= a; b ^= x;
      c -= b; x = (b>>13);
      ...
  Unfortunately, superscalar Pentiums and Sparcs can't take advantage
  of that parallelism.  They've also turned some of those single-cycle
  latency instructions into multi-cycle latency instructions.  Still,
  this is the fastest good hash I could find.  There were about 2^^68
  to choose from.  I only looked at a billion or so.
--------------------------------------------------------------------
*/
#define mix(a,b,c){ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}
      
/*
--------------------------------------------------------------------
hash() -- hash a variable-length key into a 32-bit value
  k       : the key (the unaligned variable-length array of bytes)
  len     : the length of the key, counting by bytes
  initval : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  Every 1-bit and 2-bit delta achieves avalanche.  
About 6*len+35 instructions.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 32 bits,
use a bitmask.  For example, if you need only 10 bits, do
  h = (h & hashmask(10));
In which case, the hash table should have HASHSIZE(10) elements.
  
If you are hashing n strings (ub1 **)k, do it like this:
  for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);
   
By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free.
      
See http://burtleburtle.net/bob/hash/evahash.html
Use for hash table lookup, or anything where one collision in 2^^32 is
acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
*/  

#ifdef __cplusplus
extern "C" {
#endif

/* Create hash table of hsize size and return pointer to it */
h_t *hc(int hsize);

/* Search element with key and key_length and return pointer to data */
/* key_length in bytes */

void *hs(char *key, int key_length, h_t *ht);

/* Insert element in ht pointed by data with key and key_length */
/* void *data can be just an integer*/
void *hi(char *key, int key_length, void *data, h_t *ht);

/* Destroy hash tables and free contained elements if is_pointer*/
/* if void*data is a pointer -> is_pointer=1 */
void hd(h_t *ht, int is_pointer);

#ifdef __cplusplus
}
#endif
