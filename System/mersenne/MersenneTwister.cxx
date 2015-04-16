/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   mersenne/MersenneTwister.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <cstdio>
#include <climits>
#include <iostream>
#include <cmath>
#include <time.h>

#include "MersenneTwister.h"

std::istream& 
operator>>(std::istream& IX,MTRand& mtrand)
  /*!
    Input from a stream
    \param IX :: input stream
    \param mtrand :: Random number generator
   */
{
  mtrand.read(IX);
  return IX;
}

std::ostream& 
operator<<(std::ostream& OX,const MTRand& mtrand)
  /*!
    Wite to a stream
    \param OX :: input stream
    \param mtrand :: Random number generator
   */
{
  mtrand.write(OX);
  return OX;
}

MTRand::uint32 
MTRand::hash(time_t timeValue, clock_t clockValue)
 /*!
   Get a uint32 from t and c
   Better than uint32(x) in case x is floating point in [0,1]
   Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)
   \param timeValue :: time (seconds)
   \param clockValue :: clock time 
   \return an int hash
*/
{
  static uint32 differ = 0;  // guarantee time-based seeds will change
  
  uint32 h1 = 0;
  unsigned char *p = (unsigned char *) &timeValue;
  for(size_t i = 0; i < sizeof(timeValue); ++i)
    {
      h1 *= UCHAR_MAX + 2U;
      h1 += p[i];
    }
  uint32 h2 = 0;
  p = (unsigned char *) &clockValue;
  for(size_t j = 0; j < sizeof(clockValue); ++j)
    {
      h2 *= UCHAR_MAX + 2U;
      h2 += p[j];
    }
  return (h1 + differ++) ^ h2;
}

MTRand::MTRand(const uint32& oneSeed) 
/*!
  Constructor
  \param oneSeed :: Seed start number
*/
{ 
  seed(oneSeed); 
}

MTRand::MTRand(uint32 *const bigSeed,const uint32 seedLength)  
  /*! 
    Constructor with long seed set 
    \param bigSeed :: full seeding table
    \param seedLength :: number of uint32 blocks in bigSeed
  */
{ 
  seed(bigSeed,seedLength); 
}

MTRand::MTRand()
  /*!
    Default constructor
  */
{ 
  seed(); 
}

double 
MTRand::rand()
  /*!
    Generate a randome nubmer between (0-1)
    \return (0-1)
  */
{ 
  return double(randInt()) * (1.0/4294967295.0); 
}

double 
MTRand::rand(const double& n)  
  /*!
    Generate a random number up to n
    \param n :: number to scale by
    \return (0-n)
  */
{ 
  return rand() * n; 
}

double 
MTRand::randExc()
 /*!
   Get a double precision number between (0-1)
   Excluding both 0 and 1
   \return (0-1)
  */
{ 
  return double(randInt()) * (1.0/4294967296.0); 
}

double 
MTRand::randExc(const double& n)
 /*!
   Get a double precision number between (0-n)
   Excluding both 0 and n
   \param n :: Size of random range
   \return (0-n)
  */
{ 
  return randExc() * n; 
}

double 
MTRand::randDblExc()
 /*!
   Get a double precision number between (0-1)
   Excluding both 0 and 1
   \return (0-1)
  */
{ 
  return ( static_cast<double>(randInt()) + 0.5 ) * (1.0/4294967296.0); 
}

double 
MTRand::randDblExc(const double& n)
  /*!
    Access to a double number
    Excluding both 0 and n
    \param n :: Size of number to get
    \return (0-n)
   */
{ 
  return randDblExc() * n; 
}

double 
MTRand::rand53()
  /*!
    Quick way to extra a double precision number
    \return a random double
  */
{
  uint32 a = randInt() >> 5, b = randInt() >> 6;
  return ( a * 67108864.0 + b ) * (1.0/9007199254740992.0);  // by Isaku Wada
}

double 
MTRand::randNorm(const double& mean,const double& variance)
  /*!
    Return a real number from a normal (Gaussian) distribution with given
    mean and variance by Box-Muller method
    \param mean :: Mean value
    \param variance :: varance (sigma)
    \return normal number around means 
  */
{
  const double r = sqrt( -2.0 * log( 1.0-randDblExc()) ) * variance;
  const double phi = static_cast<double>
    (2.0L * 3.14159265358979323846264338328L * randExc());
  return mean + r * cos(phi);
}

MTRand::uint32 
MTRand::randInt()
{
  // Pull a 32-bit integer from the generator state
  // Every other access function simply transforms the numbers extracted here
	
  if(left == 0) reload();
  --left;
		
  uint32 s1;
  s1 = *pNext++;
  s1 ^= (s1 >> 11);
  s1 ^= (s1 <<  7) & 0x9d2c5680U;
  s1 ^= (s1 << 15) & 0xefc60000U;
  return (s1 ^ (s1 >> 18));
}

MTRand::uint32 
MTRand::randInt(const uint32& n)
{
  // Find which bits are used in n
  // Optimized by Magnus Jonsson (magnus@smartelectronix.com)
  uint32 used = n;
  used |= used >> 1;
  used |= used >> 2;
  used |= used >> 4;
  used |= used >> 8;
  used |= used >> 16;
	
  // Draw numbers until one is found in [0,n]
  uint32 i;
  do
    i = randInt() & used;  // toss unused bits to shorten search
  while(i > n);
  return i;
}



void 
MTRand::seed(const uint32 oneSeed)
  /*!
    Set the size and initialize
    \param oneSeed
  */
{
  // Seed the generator with a simple uint32
  initialize(oneSeed);
  reload();
}


void 
MTRand::seed(uint32 *const bigSeed, const uint32 seedLength)
 /*!
   Seed the generator with an array of uint32's
   There are 2^19937-1 possible initial states.  This function allows
   all of those to be accessed by providing at least 19937 bits (with a
   default seed length of N = 624 uint32's).  Any bits above the lower 32
   in each element are discarded.
   Just call seed() if you want to get array from /dev/urandom
   \param bigSeed :: Seed value as integer list
   \param seedLength :: number of int in bigSeed array
 */
{
  initialize(19650218UL);
  int i = 1;
  uint32 j = 0;
  int k = (N > seedLength) ? static_cast<int>(N) : seedLength;
  for( ; k; --k )
    {
      state[i] =
      state[i] ^ ( (state[i-1] ^ (state[i-1] >> 30)) * 1664525U );
      state[i] += ( bigSeed[j] & 0xffffffffU ) + j;
      state[i] &= 0xffffffffU;
      ++i;  ++j;
      if( i >= N ) { state[0] = state[N-1];  i = 1; }
      if( j >= seedLength ) j = 0;
    }
  for( k = N - 1; k; --k )
    {
      state[i] =
      state[i] ^ ( (state[i-1] ^ (state[i-1] >> 30)) * 1566083941U );
      state[i] -= i;
      state[i] &= 0xffffffffU;
      ++i;
      if( i >= N ) { state[0] = state[N-1];  i = 1; }
    }
  state[0] = 0x80000000U;  // MSB is 1, assuring non-zero initial array
  reload();
}


void 
MTRand::seed()
/*!
  Seed the generator with an array from /dev/urandom if available
  Otherwise use a hash of time() and clock() values
  First try getting an array from /dev/urandom
*/
{
  FILE* urandom = fopen("/dev/urandom", "rb");
  if(urandom)
    {
      uint32 bigSeed[N];
      uint32 *s = bigSeed;
      int i = N;
      bool success = true;
      while( success && i-- )
	success = fread(s++, sizeof(uint32), 1, urandom );
      fclose(urandom);
      if(success) { seed(bigSeed, N);  return; }
    }
  
  // Was not successful, so use time() and clock() instead
  seed(hash(time(NULL), clock() ));
}


void 
MTRand::initialize(const uint32 seed)
  /*!
    Initialize generator state with seed
    See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
    In previous versions, most significant bits (MSBs) of the seed affect
    only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.
    \param seed :: New seed value
  */
{
  uint32 *s = state;
  uint32 *r = state;
  int i = 1;
  *s++ = seed & 0xffffffffUL;
  for( ; i < N; ++i )
    {
      *s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
      r++;
    }
  return;
}

void 
MTRand::reload()
  /*!
    Generate N new values in state
    Made clearer and faster by Matthew Bellew (matthew.bellew\@home.com)
  */
{
  uint32 *p = state;
  int i;
  for(i = N - M; i--; ++p)
    *p = twist(p[M], p[0], p[1]);
  for(i = M; --i; ++p)
    *p = twist(p[M-N], p[0], p[1]);
  *p = twist(p[M-N], p[0], state[0]);
  
  left = N;
  pNext = state;
  return;
}


void 
MTRand::save(uint32* saveArray) const
  /*!
    Save the state of the random number generator
    \param saveArray :: Array to place data [must be pre-allocated]
  */
{
  uint32 *sa = saveArray;
  const uint32 *s = state;
  int i = N;
  for(; i--; *sa++ = *s++) {}
  *sa = left;
}


void 
MTRand::load(uint32 *const loadArray)
  /*!
    Load the state of the random number generator
    \param loadArray :: Array to get data [N Items long]
  */
{
  uint32 *s = state;
  uint32 *la = loadArray;
  int i = N;
  for(; i--; *s++ = *la++) {}
  left = *la;
  pNext = &state[N-left];
}


void 
MTRand::write(std::ostream& OX) const
  /*!
    Output the state of the system to a stream
    \param OX :: Output stream
  */
{
  const MTRand::uint32 *s = state;
  int i = N;
  for( ; i--; OX << *s++ << "\t") {}
  OX << left;
  OX<<std::endl;
  return;
}


void
MTRand::read(std::istream& IX)
  /*!
    Input the state of the system to a stream
    \param IX :: Input stream
  */
{
  MTRand::uint32 *s = state;
  int i = N;
  for( ; i--; IX >> *s++) {}
  IX >> left;
  pNext = &state[N-left];
  return;
}



