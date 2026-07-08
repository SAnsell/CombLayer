/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   modelSupport/masterWrite.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <algorithm>
#include <fmt/core.h>

#include "Exception.h"
#include "Vec3D.h"
#include "masterWrite.h"

masterWrite::masterWrite() :
  zeroTol(1e-20),sigFig(6),
  FMTdouble("{:.6g}"),
  FMTinteger("{:d}")
  /*!
    Constructor
  */
{}

masterWrite::~masterWrite()
 /*!
   Destructor
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
masterWrite::setSigFig(const size_t S)
  /*!
    Sets the number of significant figures
    \param S :: Significant figures
  */
{
  if (S==0)
    throw ColErr::IndexError<size_t>(S,0,"masterWrite::setSigFig");
  std::ostringstream cx;
  cx<<"{:."<<S<<"g}";
  sigFig=S;
  FMTdouble=cx.str();
  return;
}

void
masterWrite::setZero(const double Z)
  /*!
    Sets the Zero lefe
    \param Z :: Zero
  */
{
  zeroTol=std::abs(Z);
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
  if (std::abs(D)<zeroTol)
    return "0.0";

  return fmt::format(fmt::runtime(FMTdouble),D);
}

std::string
masterWrite::Num(const int& I)
  /*!
    Write out a specific double
    \param I :: integer to write
    \return formated number
  */
{
  return fmt::format(fmt::runtime(FMTinteger),I);
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
      Out +=(std::abs(V[i])<zeroTol) ? "0.0" :
	fmt::format(fmt::runtime(FMTdouble),V[i]);
      if (i!=2) Out+=" ";
    }
  return Out;
}

std::string
masterWrite::NumComma(const Geometry::Vec3D& V)
  /*!
    Write out a specific double
    \param V :: Vector to write
    \return formated number
  */
{
  std::string Out;
  for(int i=0;i<3;i++)
    {
      Out +=(std::abs(V[i])<zeroTol) ? "0.0" :
	fmt::format(fmt::runtime(FMTdouble),V[i]);
      if (i!=2) Out+=",";
    }
  return Out;
}

std::string
masterWrite::NameNoDot(std::string V)
  /*!
    Write out a name without symbols forbidden in POV-Ray
    \param V :: Original name
    \return formated name
  */
{
  std::replace(V.begin(),V.end(),'.','d');
  std::replace(V.begin(),V.end(),'%','p');
  std::replace(V.begin(),V.end(),'#','h');
  std::replace(V.begin(),V.end(),'-','_');

  return V;
}


template<typename T>
std::string
masterWrite::padNum(const T& V,const size_t len)
  /*!
    Write out a padded string
    \param V :: Value
    \param len :: length
    \return Num(T) + spaces to length
   */
{
  std::string Out=Num(V);
  if (Out.size()<len)
    Out+=std::string(len-Out.size(),' ');
  return Out;
}

template<>
std::string
masterWrite::padNum(const Geometry::Vec3D& V,const size_t len)
  /*!
    Write out a padded string for a vec
    \param V :: Value
    \param len :: length of each component
    \return Num(Vec3D) + spaces to length
   */
{
  std::string Out;
  for(size_t i=0;i<3;i++)
    {
      Out += padNum(V[i],len);
      if (i!=2) Out+=" ";
    }
  return Out;

}

///\cond TEMPLATE

template std::string masterWrite::padNum(const double&,const size_t);
template std::string masterWrite::padNum(const int&,const size_t);

///\endcond TEMPLATE
