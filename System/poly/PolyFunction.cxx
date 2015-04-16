/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   poly/PolyFunction.cxx
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
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

#include "Exception.h"
#include "support.h"
#include "PolyFunction.h"

namespace mathLevel
{

std::ostream& 
operator<<(std::ostream& OX,const PolyFunction& A)
  /*!
    External Friend :: outputs point to a stream 
    \param OX :: output stream
    \param A :: PolyFunction to write
    \returns The output stream (OX)
  */
{
  (&A)->write(OX);
  return OX;
}

PolyFunction::PolyFunction() : 
  Eaccuracy(1e-6)
  /*!
    Constructor 
  */
{}

PolyFunction::PolyFunction(const double E) : 
  Eaccuracy(fabs(E))
  /*!
    Constructor 
    \param E :: Accuracy
  */
{}

PolyFunction::PolyFunction(const PolyFunction& A)  :
  Eaccuracy(A.Eaccuracy)
  /*! 
    Copy Constructor
    \param A :: PolyFunction to copy
   */
{ }

PolyFunction& 
PolyFunction::operator=(const PolyFunction& A)
  /*! 
    Assignment operator
    \param A :: PolyFunction to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Eaccuracy=A.Eaccuracy;
    }
  return *this;
}

PolyFunction::~PolyFunction()
  /// Destructor
{}

size_t
PolyFunction::getMaxSize(const std::string& CLine,const char V)
  /*!
    Finds the maximum power in the string
    of the variable type
    \param CLine :: Line to calcuate V^x componenets
    \param V :: Variable letter.
    \return Max Power 
  */
{
  size_t maxPower(0);
  std::string::size_type pos(0);
  pos=CLine.find(V,pos);
  const std::string::size_type L=CLine.length()-2;
  while(pos!=std::string::npos)
    {
      if (pos!=L && CLine[pos+1]=='^') 
        {
	  size_t pV;
	  if (StrFunc::convPartNum(CLine.substr(pos+2),pV) && pV>maxPower)
	    maxPower=pV;
	}
      else if (!maxPower)      // case of +y+... etc
	maxPower=1;
	
      pos=CLine.find(V,pos+1); 
    }
  return maxPower;
}

}  // NAMESPACE  mathLevel

