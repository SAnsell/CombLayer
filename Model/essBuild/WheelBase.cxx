/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/WheelBase.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "WheelBase.h"


namespace essSystem
{

WheelBase::WheelBase(const std::string& Key) :
  attachSystem::FixedRotate(Key,16),
  attachSystem::ContainedGroup("Wheel","Shaft"),
  attachSystem::CellMap(),
  engActive(0)
  /*!
    Constructor
    There are 10 links possible -- last for are used if BilbaoWheel used..
    \param Key :: Name of construction key
  */
{}

WheelBase::WheelBase(const WheelBase& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),
  engActive(A.engActive)
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
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      engActive=A.engActive;
    }
  return *this;
}


WheelBase::~WheelBase()
  /*!
    Destructor
   */
{}

  
} // NAMESPACE essSystem
