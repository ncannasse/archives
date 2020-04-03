#ifndef MTSRANDOM_H
#define MTSRANDOM_H

typedef struct _rnd rnd;

int rnd_size();
rnd *rnd_init( void *data );
void rnd_set_seed( rnd *r, int seed );
unsigned int rnd_int( rnd *r );
double rnd_rand_float( rnd *r );

#endif
