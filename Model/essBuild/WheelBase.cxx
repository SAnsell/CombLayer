/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/WheelBase.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "WheelBase.h"


namespace essSystem
{

WheelBase::WheelBase(const std::string& Key) :
  attachSystem::ContainedGroup("Wheel","Shaft"),
  attachSystem::FixedOffset(Key,13),
  attachSystem::CellMap()
  /*!
    Constructor
    There are 10 links possible -- last for are used if BilbaoWheel used..
    \param Key :: Name of construction key
  */
{}

WheelBase::WheelBase(const WheelBase& A) : 
  attachSystem::ContainedGroup(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A)
  /*!
    Copy constructor
    \param A :: WheelBase to copy
  */
{}

WheelBase&
WheelBase::operator=(const WheelBase& A)
  /*!
    Assignment operator
    \param A :: WheelBase to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
    }
  return *this;
}


WheelBase::~WheelBase()
  /*!
    Destructor
   */
{}

void
WheelBase::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: sideIndex
  */
{
  ELog::RegMethod RegA("WheelBase","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}
  
} // NAMESPACE essSystem
