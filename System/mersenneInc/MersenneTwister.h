/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   mersenneInc/MersenneTwister.h
*
 * Copyright (c) 2004-2016 by Stuart Ansell
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
// MersenneTwister.h
// Mersenne Twister random number generator -- a C++ class MTRand
// Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
// Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

// The Mersenne Twister is an algorithm for generating random numbers.  It
// was designed with consideration of the flaws in various other generators.
// The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
// are far greater.  The generator is also fast; it avoids multiplication and
// division, and it benefits from caches and pipelines.  For more information
// see the inventors' web page at http://www.math.keio.ac.jp/~matumoto/emt.html

// Reference
// M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
// Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
// Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// Copyright (C) 2000 - 2003, Richard J. Wagner
// All rights reserved.                          
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//   3. The names of its contributors may not be used to endorse or promote 
//      products derived from this software without specific prior written 
//      permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// The original code included the following notice:
//
//     When you use this, send an email to: matumoto@math.keio.ac.jp
//     with an appropriate reference to your work.
//
// It would be nice to CC: rjwagner@writeme.com and Cokus@math.washington.edu
// when you write.

#ifndef MERSENNETWISTER_H
#define MERSENNETWISTER_H

// Not thread safe (unless auto-initialization is avoided and each thread has
// its own MTRand object)

/*!
  \class MTRand 
  \version 1.0
  \author Richard J. Wagner 
  \date 15 May 2003
  \brief Processes random numbers
 */

class MTRand 
{
  // Data
 public:

  typedef unsigned int uint32;  ///< unsigned integer type, at least 32 bits
	
  enum { N = 624 };       ///< length of state vector
  enum { SAVE = N + 1 };  ///< length of array for save()

 private:

  enum { M = 397 };       ///<  period parameter

  uint32 saveState[N+1];  ///< internal state (include left)
  uint32 state[N];        ///< internal state
  uint32 *pNext;          ///< next value to get from state
  uint32 left;            ///< number of values left before reload needed

  // Methods:
  void reload();
  void initialize(const uint32);
  ///\cond SIMPLE_METHOD
  uint32 hiBit(const uint32& u) const { return u & 0x80000000U; }
  uint32 loBit(const uint32& u) const { return u & 0x00000001U; }
  uint32 loBits(const uint32& u) const { return u & 0x7fffffffU; }
  uint32 mixBits(const uint32& u,const uint32& v) const
    { return hiBit(u) | loBits(v); }
  uint32 twist(const uint32& m,const uint32& s0,const uint32& s1) const
  { return m ^ (mixBits(s0,s1)>>1) ^ (-loBit(s1) & 0x9908b0dfU); }
  ///\endcond SIMPLE_METHOD		      

  static uint32 hash(time_t,clock_t);

  //Methods
  
 public:
		      
  MTRand(const uint32&);  // initialize with a simple uint32
  MTRand(uint32* const,const uint32 = N );  // or an array
  MTRand();  
		      
  // Access to 32-bit random numbers
  double rand();                          ///< real number in [0,1]
  double rand(const double&);             ///< real number in [0,n]
  double randExc();                       ///< real number in [0,1)
  double randExc(const double& n);        ///< real number in [0,n)
  double randDblExc();                    ///< real number in (0,1)
  double randDblExc(const double& n);     ///< real number in (0,n)
  uint32 randInt();                       ///< integer in [0,2^32-1]
  uint32 randInt(const uint32&);          ///< integer in [0,n] for n < 2^32
  double operator()() { return rand(); }  ///< real number in [0,1]
	
  // Access to 53-bit random numbers (capacity of IEEE double precision)
  double rand53();  // real number in [0,1)
	
  // Access to nonuniform random number distributions
  double randNorm( const double& mean = 0.0, const double& variance = 1.0 );
	
  // Re-seeding functions with same behavior as initializers
  void seed(const uint32);
  void seed(uint32* const,const uint32 seedLength = N );
  void seed();
	
  // Saving and loading generator state
  void createSave();
  void loadSave();
  void save(uint32*) const;  // to array of size SAVE
  void load(uint32* const);  // from such array
		      
  void read(std::istream&);
  void write(std::ostream&) const;

};

std::istream& operator>>(std::istream& is,MTRand&);
std::ostream&  operator<<(std::ostream&,const MTRand&);


#endif  // MERSENNETWISTER_H
