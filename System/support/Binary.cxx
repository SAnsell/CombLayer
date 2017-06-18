/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/Binary.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Binary.h"


std::ostream&
operator<<(std::ostream& OX,const Binary& A)
  /*!
    Ouptput to a stream			       
    \param OX :: Output stream
    \param A :: Binary object to write
    \return stream state
  */
{
  A.write(OX);
  return OX;
}

Binary::Binary() :
  num(0),pad(0)
  /*!
    Constructor
  */
{}

Binary::Binary(const unsigned long int& A,const int P) :
  num(A),pad(P)
  /*!
    Constructor
    \param A :: Input number 
    \param P :: padding number
  */
{}

Binary::Binary(const std::string& A,const int P) :
  pad(P)
  /*!
    Constructor 
    \param A :: Input number  as string
    \param P :: padding number
  */
{
  num=0;
  for(size_t i=0;i<A.size();i++)
    {
      switch (A[i]) 
        {
	case '0':
	  num <<= 1;
	  break;
	case '1':
	  num <<= 1;
	  num+=1;
	}
    }
}

Binary::Binary(const char* A,const unsigned int S,const int P) :
  pad(P)
  /*!
    Constructor 
    \param A :: Input number  as string
    \param S :: Size of character array
    \param P :: padding number
  */
{
  num=0;
  size_t i;
  for(i=0;i<S && i<4;i++)
    {
      num*=256;
      num+=static_cast<unsigned char>(A[i]);
    }
  for(;i<4;i++)
    num*=256;
}

Binary::Binary(const unsigned char* A,const unsigned int S,const int P) :
  pad(P)
  /*!
    Constructor 
    \param A :: Input number  as string
    \param S :: Size of character array
    \param P :: padding number
  */
{
  num=0;
  size_t i;
  for(i=0;i<S && i<4;i++)
    {
      num*=256;
      num+=A[i];
    }
  for(;i<4;i++)
    num*=256;
}

Binary::Binary(const std::string& A,const unsigned int S,const int P) :
  pad(P)
  /*!
    Constructor 
    \param A :: Input number  as string
    \param S :: Size of character array
    \param P :: padding number
  */
{
  num=0;
  size_t i;
  for(i=0;i<S && i<4 && i<A.size();i++)
    {
      num*=256;
      num+=static_cast<unsigned long int>(A[i]);
    }
  for(;i<4;i++)
    num*=256;
}

Binary::Binary(const std::vector<int>& A,const int P)  : 
  pad(P)
  /*!  
    Sets parts on a vector of flags : 
    Zero point is the high bit ?
    \param A :: Vector of flags (lowBit->highBit)
    \param P :: padding number
  */
{
  num=0;
  const int Sz=static_cast<int>(A.size());
  unsigned long int part= (1 << (Sz-1));
  std::vector<int>::const_iterator vc;
  for(vc=A.begin();vc!=A.end();vc++)
    {
      if (*vc)
	num+=part;
      part >>= 1;
    }
}

Binary::Binary(const Binary& A) : 
  num(A.num),pad(A.pad)
  /*!
    Copy constuctor
    \param A :: number to copy
  */
{}

Binary&
Binary::operator=(const Binary& A) 
  /*!
    Standard Assignment operator
    \param A :: Object to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      num=A.num;
      pad=A.pad;
    }
  return *this;
}

Binary::~Binary()
  /*! 
    Destructor
  */
{}

void
Binary::setNum(const std::string& A)
 /*!
   Sets the binary number based on 4 characters offstring
   \param A :: string [high:low]
  */
{
  size_t i;
  num=0;
  for(i=0;i<4 && i<A.size();i++)
    {
      num*=256;
      num=static_cast<unsigned int>(A[i]);
    }
  for(;i<4;i++)
    num*=256;
  return;
}


void
Binary::setNum(const unsigned long int& A)
 /*!
   Sets the binary number based on number
   \param A :: Number
  */
{
  num=A;
  return;
}
  
void
Binary::write(std::ostream& OX) const
  /*!
    Write out to a stream (uses pad)
    \param OX :: Output steam
  */
{
  write(OX,pad);
  return;
}

void
Binary::write(std::ostream& OX,const int size) const
  /*!
    Write out to a stream (uses size)
    \param OX :: Output steam
    \param size :: Size of padding
  */
{
  std::vector<int> Bits;
  unsigned long int nval(num);
  while(nval)
    {
      Bits.push_back(static_cast<int>(nval & 1));
      nval >>= 1;
    }
  const char* OutVal={"01"};

  if (size)
    for(int i=0;i<size-static_cast<int>(Bits.size());i++)
      OX<<"0";

  std::vector<int>::reverse_iterator rc;
  for(rc=Bits.rbegin();rc!=Bits.rend();rc++)
    OX<<OutVal[*rc];
  return;
}
