/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/LinkedComp.cxx
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
#include <deque>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "Line.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "LinkedComp.h"

namespace attachSystem
{

LinkedComp::LinkedComp(const int N) :
  LU(N)
  /*!
    Constructor 
    \param N :: Size of Array
  */
{}

LinkedComp::LinkedComp(const LinkedComp& A) : 
  LU(A.LU)
  /*!
    Copy constructor
    \param A :: LinkedComp to copy
  */
{}

LinkedComp&
LinkedComp::operator=(const LinkedComp& A)
  /*!
    Assignment operator
    \param A :: LinkedComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      LU=A.LU;
    }
  return *this;
}

LinkedComp::~LinkedComp()
  /*!
    Deletion operator
  */
{}


void
LinkedComp::setAxis(const int Index,const Geometry::Vec3D& xv,
		    const Geometry::Vec3D& yv,const Geometry::Vec3D& zv)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param xv :: X coordinate
   \param yv :: Y coordinate
   \param zv :: Z coordinate
 */
{
  ELog::RegMethod RegA("LinkedComp","setAxis");
  if (Index<0 || Index>static_cast<int>(LU.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(LU.size()),
				  RegA.getFull());

  LU[Index].setAxis(xv,yv,zv);
  return;
}

void
LinkedComp::setCentre(const int Index,const Geometry::Vec3D& C)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param C :: Centre coordinate
 */
{
  ELog::RegMethod RegA("LinkedComp","setCentre");
  if (Index<0 || Index>static_cast<int>(LU.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(LU.size()),
				  RegA.getFull());

  LU[Index].setCentre(C);
  return;
}


void
LinkedComp::setExtent(const int Index,const double xE,
		      const double yE,const double zE)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param xE :: Extent in X 
   \param yE :: Extent in Y 
   \param zE :: Extent in Z 
   
 */
{
  ELog::RegMethod RegA("LinkedComp","setExtent");
  if (Index<0 || Index>static_cast<int>(LU.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(LU.size()),
				  RegA.getFull());

  LU[Index].setExtent(xE,yE,zE);
  return;
}

const LinkUnit&
LinkedComp::getLU(const int Index) const
 /*!
   Get the axis of the linked component
   \param Index :: Link number
   \return LinkUnit reference
 */
{
  ELog::RegMethod RegA("LinkedComp","getLinkUnit");
  if (Index<0 || Index>static_cast<int>(LU.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(LU.size()),
				  RegA.getFull());

  return LU[Index];
}

int
LinkedComp::getLinkSurf(const int Index) const
  /*!
    Accessor to the link surface string
    \param Index :: Link number
    \return Surface Key number
  */
{
  ELog::RegMethod RegA("LinkedComp","getLinkSurf");
  if (Index<0 || Index>static_cast<int>(LU.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(LU.size()),
				  RegA.getFull());
  
  return LU[Index].getLinkSurf();
}


std::string
LinkedComp::getLinkString(const int Index) const
  /*!
    Accessor to the link surface string
   \param Index :: Link number
   \return String of link
   */
{
  ELog::RegMethod RegA("LinkedComp","getLinkSurf");
  if (Index<0 || Index>static_cast<int>(LU.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(LU.size()),
				  RegA.getFull());
  
  return LU[Index].getLinkString();
}

}  // NAMESPACE attachSystem
