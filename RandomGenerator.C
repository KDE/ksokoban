/* 
 * A C++ version of MT19937 (the Mersenne twister)
 *
 * This pseudorandom number generator is based on MT19937 (the Mersenne
 * twister). It was ported to C++ by Anders Widell <awl@hem.passagen.se>.
 * I also added support for generating doubles and for generating integers
 * in an interval other than [0,2^32-1]
 *
 * See http://www.math.keio.ac.jp/matumoto/emt.html for the most recent
 * information on the Mersenne twister.
 *
 * The original copyright notice follows:
 *
 */

/* 
    LICENCE CONDITIONS: 

		Matsumoto and Nishimura consent to GNU General 
		Public Licence

    NOTE: 
		When you use it in your program, please let Matsumoto
                <matumoto@math.keio.ac.jp> know it.

		Because of a machine-trouble, Matsumoto lost emails 
   		which arrived during May 28-29.

*/

/********************************************************************/

#include "RandomGenerator.H"
#include <assert.h>
#include <time.h>
#include <unistd.h>

#define RANDOM_N 624
#define RANDOM_M 397
#define MATRIX_A 0x9908b0dfu           /* constant vector a */
#define UPPER_MASK 0x80000000u         /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffu         /* least significant r bits */

/* mag01[x] = x * MATRIX_A  for x=0,1 */
static const unsigned mag01[2] = {0x0u, MATRIX_A};

/**********************************************************************/
/* Create a pseudorandom number generator and initialise it with seed */
/**********************************************************************/
RandomGenerator::RandomGenerator(unsigned seed) {
  initialise(seed);
}

RandomGenerator::RandomGenerator() {
  initialise(0xc88fb833u * time(0) * getpid() * getppid());
}


/********************************************************************/
/* Function for seeding the random number generator.                */
/********************************************************************/
void 
RandomGenerator::initialise(unsigned seed) {
  /* setting initial seeds to ptgfsr[RANDOM_N] using     */
  /* the generator Line 25 of Table 1 in                 */
  /* [KNUTH 1981, The Art of Computer Programming        */
  /*    Vol. 2 (2nd Ed.), pp102]                         */

  ptgfsr[0] = seed & 0xffffffffu;
  if (ptgfsr[0] == 0u) ptgfsr[0] = 0x7be56ae5u; // We don't allow seed==0

  for (register int i=1; i<RANDOM_N; i++)
    ptgfsr[i] = (69069u * ptgfsr[i-1]) & 0xffffffffu;

  k = RANDOM_N;
}

/************************************************************************/
/* Function for generating a table-full of random numbers, all at once. */
/************************************************************************/
void
RandomGenerator::genrand_function() {
  register unsigned y;
  register int kk;

  for (kk=0; kk < RANDOM_N-RANDOM_M; kk++) {
    y = (ptgfsr[kk]&UPPER_MASK)|(ptgfsr[kk+1]&LOWER_MASK);
    ptgfsr[kk] = ptgfsr[kk+RANDOM_M] ^ (y >> 1) ^ mag01[y & 0x1];
  }
  for (; kk < RANDOM_N-1; kk++) {
    y = (ptgfsr[kk]&UPPER_MASK)|(ptgfsr[kk+1]&LOWER_MASK);
    ptgfsr[kk] = ptgfsr[kk+(RANDOM_M-RANDOM_N)] ^ (y >> 1) ^ mag01[y & 0x1];
  }
  y = (ptgfsr[RANDOM_N-1]&UPPER_MASK)|(ptgfsr[0]&LOWER_MASK);
  ptgfsr[RANDOM_N-1] = ptgfsr[RANDOM_M-1] ^ (y >> 1) ^ mag01[y & 0x1];
  
  k = 0;
}

/************************************************************************/

unsigned
RandomGenerator::next(unsigned n) {
  assert(n > 0);

  if ((n & -n) == n) return next() & (n-1);

  register unsigned rnd, unused = -((-n & 0xffffffffu) % n) & 0xffffffffu;
  do {
    rnd = next();
  } while (rnd >= unused);
  
  return rnd % n;
}

double
RandomGenerator::nextDouble() {
  register unsigned hi, lo;

  do {
    hi = next() & 2097151u;
    lo = next();
  } while ((hi|lo) == 0);

  return (hi * 4294967296.0 + lo) / 2097152.0 / 4294967296.0;
}

