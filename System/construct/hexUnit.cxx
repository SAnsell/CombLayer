/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/hexUnit.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "gridUnit.h"
#include "hexUnit.h"

namespace constructSystem
{

hexUnit::hexUnit(const int aI,const int bI,
		   const Geometry::Vec3D& C) :
  gridUnit(6,aI,bI,C)
  /*!
    Constructor
    \param aI :: Index A
    \param bI :: Index B
    \param C :: Centre point
  */
{}

hexUnit::hexUnit(const int aI,const int bI,const bool cF,
		   const Geometry::Vec3D& C) : 
  gridUnit(6,aI,bI,cF,C)
  /*!
    Constructor
    \param aI :: Index A
    \param bI :: Index B
    \param cF :: Cut flag
    \param C :: Centre point
  */
{}

hexUnit::hexUnit(const hexUnit& A) :
  gridUnit(A)
  /*!
    Copy constructor
    \param A :: hexUnit to copy
  */
{}

hexUnit&
hexUnit::operator=(const hexUnit& A)
  /*!
    Assignment operator
    \param A :: hexUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      gridUnit::operator=(A);
    }
  return *this;
}

int
hexUnit::hexIndex(const size_t index)
  /*!
    Convert Index[0-5] into an modification of the base index
    \param index :: value between 0 and 5 
    \return 1000 point offset index
   */
{
  switch (index % 6) 
    {
    case 0:
      return 1000;   // 1,0
    case 1:
      return 1;    // 0,1
    case 2:
      return -999;     // -1,1
    case 3:
      return -1000;  // -1,0 
    case 4:
      return -1;   // -1,1
    case 5:
      return 999;      //  1,-1
    }
  // return to avoid compiler warning
  return 0;
}




bool 
hexUnit::isConnected(const gridUnit& TU) const 
  /*!
    Determine if a point is connected. 
    \param TU :: Tube unit to check
    \return true if within one unit
  */
{
  const int dA=iA-TU.getAIndex();
  const int dB=iB-TU.getBIndex();
  if ((dA*dB==0 || dA+dB==0) && (dA==1 || dB==1 || dB==-1 || dA==-1)) 
    return 1;

  return 0;
}
  
}  // NAMESPACE constructSystem
