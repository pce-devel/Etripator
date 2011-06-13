#ifndef RANDOM_H
#define RANDOM_H

/** \brief George Marsaglia's random number generator.
 *  Its name stands for Complimentary-Multiply-With-Carry
 *  4096 is the initial random value.
 *
 *  Reference : 
 *		* http://groups.google.com/group/comp.lang.c/browse_thread/thread/a9915080a4424068/?pli=1
 *		* http://groups.google.com/group/sci.crypt/browse_thread/thread/305c507efbe85be4?pli=1
 */
unsigned long CMWC4096(void);

/** \brief Initialize CMWC496 RNG */
void SetupCMWC4096(void);

#endif // RANDOM_H