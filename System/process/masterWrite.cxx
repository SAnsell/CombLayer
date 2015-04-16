/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/masterWrite.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <boost/format.hpp>

#include "Exception.h"
#include "Vec3D.h"
#include "masterWrite.h"

masterWrite::masterWrite() :
  zeroTol(1e-20),sigFig(6),
  FMTdouble("%1.6g"),
  FMTinteger("%1d")
  /*!
    Constructor
  */
{}

masterWrite&
masterWrite::Instance()
  /*!
    Singleton this
    \return masterWrite object
   */
{
  static masterWrite MR;
  return MR;
}

void
masterWrite::setSigFig(const int S)
  /*!
    Sets the number of significant figures
    \param S :: Significant figures
  */
{
  if (S<=0)
    throw ColErr::IndexError<int>(S,0,"masterWrite::setSigFig");
  std::ostringstream cx;
  cx<<"\%1."<<S<<"g";
  FMTdouble=boost::format(cx.str());
  return;
}

void
masterWrite::setZero(const double Z)
  /*!
    Sets the Zero lefe
    \param Z :: Zero
  */
{
  zeroTol=fabs(Z);
  return;
}

std::string
masterWrite::Num(const double& D)
  /*!
    Write out a specific double
    \param D :: number to process
    \return formated number / 0.0 
   */
{
  if (fabs(D)<zeroTol)
    return "0.0";

  return (FMTdouble % D).str();
}
  
std::string
masterWrite::Num(const int& I)
  /*!
    Write out a specific double
    \param I :: integer to write
    \return formated number
  */
{
  return (FMTinteger % I).str();
}

std::string
masterWrite::Num(const Geometry::Vec3D& V)
  /*!
    Write out a specific double
    \param V :: Vector to write
    \return formated number
  */
{
  std::string Out;
  for(int i=0;i<3;i++)
    {
      Out +=(fabs(V[i])<zeroTol) ? "0.0" :
	(FMTdouble % V[i]).str();
      if (i!=2) Out+=" ";
    }
  return Out;
}

