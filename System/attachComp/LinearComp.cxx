/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/LinearComp.cxx
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
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfEqual.h"
#include "Rules.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"

namespace attachSystem
{

LinearComp::LinearComp(const std::string& KN)  :
  FixedComp(KN,2)
  /*!
    Constructor 
    \param KN :: KeyName
  */
{}

LinearComp::LinearComp(const LinearComp& A) : 
  FixedComp(A)
  /*!
    Copy constructor
    \param A :: LinearComp to copy
  */
{}

LinearComp&
LinearComp::operator=(const LinearComp& A)
  /*!
    Assignment operator
    \param A :: LinearComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FixedComp::operator=(A);
    }
  return *this;
}

void
LinearComp::createUnitBeamVector(const LinearComp& LC)
  /*!
    Create the unit vectors [using beam directions]
    \param LC :: Linear Comp
  */
{
  ELog::RegMethod RegA("LinearComp","createUnitBeamVector");

  Z=Geometry::Vec3D(-1,0,0);          // Gravity axis [up]
  Y=LC.getBeamAxis();
  X=(Y*Z);                            // horrizontal axis [across]

  Origin=LC.getBeamExit();
  return;
}  

const Geometry::Vec3D&
LinearComp::getExitNorm() const
  /*!
    Get Exit normal if set / Default to Y
    \return Direction Vector
  */
{
  return (LU[1].hasAxis())  ? 
    LU[1].getAxis() : Y;
}

const Geometry::Vec3D&
LinearComp::getExit() const
  /*!
    Get Exit if set / Default to Origin
    \return Exit point
  */
{
  return (LU[1].hasConnectPt())  ? 
    LU[1].getConnectPt() : Origin;
}

int 
LinearComp::getExitSurface() const
  /*!
    Return linked surface [currently not checked]
    \return LinkSurface
  */
{
  return LU[1].getLinkSurf();
}
  
void
LinearComp::setExit(const Geometry::Vec3D& C,
		    const Geometry::Vec3D& A) 
  /*!
    User Interface to LU[1] to set Point + Axis
    \param C :: Connect point
    \param A :: Axis
  */
{
  LU[1].setAxis(A);
  LU[1].setConnectPt(C);
  return;
}

void
LinearComp::setExitSurf(const int SN)
  /*!
    Set the exit surface
    \param SN :: Surface number
   */
{
  LU[1].setBridgeSurf(SN);
  return;
}

}  // NAMESPACE attachSystem
