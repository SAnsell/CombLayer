/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/LayerInfo.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "LayerInfo.h"

namespace ts1System
{

LayerInfo::LayerInfo(const double A,const double B,
		     const double C,const double D,
		     const double E,const double F)
/*!
  Ugly constructor
  \param A :: Front/Back
  \param B :: Front/Back
  \param C :: Sides
  \param D :: Sides
  \param E :: Top/Bottom
  \param F :: Top/Bottom
*/
{
  V[0]=A;
  V[1]=B;
  V[2]=C;
  V[3]=D;
  V[4]=E;
  V[5]=F;
  V[6]=0.0;
  V[7]=0.0;
  V[8]=0.0;
}

LayerInfo::LayerInfo(const double A,const double B,
		     const double C)
/*!
  Ugly constructor
  \param A :: Front/Back by 2
  \param B :: Sides by 2
  \param C :: Top/Bottom by 2
*/
{
  V[0]=V[1]=A/2.0;
  V[2]=V[3]=B/2.0;  
  V[4]=V[5]=C/2.0;
  V[6]=0.0;
  V[7]=0.0;
  V[8]=0.0;
}

LayerInfo::LayerInfo(const LayerInfo& A) : 
  mat(A.mat),Temp(A.Temp)
  /*!
    Copy constructor
    \param A :: LayerInfo to copy
  */
{
  for(size_t j=0;j<9;j++)
    V[j]=A.V[j];
}

LayerInfo&
LayerInfo::operator=(const LayerInfo& A)
  /*!
    Assignment operator
    \param A :: LayerInfo to copy
    \return *this
  */
{
  if (this!=&A)
    {
      for(size_t j=0;j<9;j++)
	V[j]=A.V[j];
      mat=A.mat;
      Temp=A.Temp;
    }
  return *this;
}


LayerInfo&
LayerInfo::operator+=(const double Value) 
  /*!
    Simple addition system
    \param Value :: Value to add to all units
    \return this + A
  */
{
  // Only add non round values
  for(size_t i=0;i<6;i++)
    V[i]+=Value;
  return *this;
}

LayerInfo&
LayerInfo::operator+=(const LayerInfo& A) 
  /*!
    Simple addition system
    \param A :: LayerInfo to add
    \return this + A
  */
{
  for(size_t i=0;i<6;i++)
    V[i]+=A.V[i];
  return *this;
}

void
LayerInfo::setRounds(const double FR,const double BR,const double T)
  /*!
    Set the material 
    \param FR :: front round
    \param BR :: Back round
    \param T :: Tube rounds
   */
{
  V[6]=FR;
  V[7]=BR;
  V[8]=T;
  return;
}

void
LayerInfo::setMat(const int M,const double T)
  /*!
    Set the material 
    \param M :: Material
    \param T :: Temperature [K]
   */
{
  mat=M;
  Temp=fabs(T);
  return;
}

double 
LayerInfo::Item(const size_t Index) const
  /*!
    Accessor 
    \param Index :: Index value [0-8]
   */
{
  if (Index>8)
    {
      ELog::RegMethod RegA("LayerInfo","getValue");
      throw ColErr::IndexError<size_t>(Index,9,"Index");
    }
  return V[Index];
}



  
}  // NAMESPACE ts1System
