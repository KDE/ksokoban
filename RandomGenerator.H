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

#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

class RandomGenerator {
public:
  /**
   * Create a new generator and initialise it with a randomly chosen seed.
   * The seed will be based on the system time and the process number.
   */

  RandomGenerator();

  /**
   * Create a new generator and initialise it with the given seed.
   * seed may be any 32-bit integer.
   */

  RandomGenerator(unsigned seed);

  /**
   * Restart the random number generator with the new seed.
   * seed may be any 32-bit integer.
   */

  void initialise(unsigned seed);

  /**
   * Returns random 32-bit unsigned integers uniformly distributed
   * on the interval [0, 2^32-1]
   */

  unsigned next() {
    if (k >= 624) genrand_function();
    register unsigned y = ptgfsr[k++];
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680u;
    y ^= (y << 15) & 0xefc60000u;
    y &= 0xffffffffu;    // you may delete this line if word size == 32
    return y ^ (y >> 18);
  }

  /**
   * Skips the next 32-bit random number in the sequence. This has the same
   * effect on the generator state as next(), but is a bit faster since
   * the actual value doesn't have to be calculated.
   */

  void skip() {
    if (k >= 624) genrand_function();
    k++;
  }

  /**
   * Returns random unsigned integers uniformly distributed
   * on the interval [0, n-1]. This function will call next()
   * at least one time.
   */

  unsigned next(unsigned n);

  /**
   * Returns random doubles uniformly distributed
   * on the open interval (0, 1). This function will call next()
   * at least two times.
   */

  double nextDouble();

private:
  void genrand_function();

  int k;
  unsigned ptgfsr[624];  /* set initial seeds: RANDOM_N = 624 words */
};

#endif  /* RANDOMGENERATOR_H */
