/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/objectRegister.cxx
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
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <set>
#include <string>
#include <algorithm>
#include <numeric>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "support.h"
#include "stringCombine.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "objectRegister.h"

namespace ModelSupport
{

objectRegister::objectRegister() : 
  GPtr(0)
  /*!
    Constructor
  */
{}

objectRegister::~objectRegister() 
  /*!
    Destructor
  */
{}

  
objectRegister& 
objectRegister::Instance() 
  /*!
    Effective this object			
    \return objectRegister object
  */
{
  static objectRegister A;
  return A;
}


  
int
objectRegister::cell(const std::string& Name,const int size)
  /*!
    Add a component and get a new cell number 
    \param Name :: Name of the unit
    \param size :: Size of unit to register
    \return the start number of the cellvalue
  */
{
  ELog::RegMethod RegA("objectRegister","cell");

  if (!GPtr)
    throw ColErr::EmptyValue<objectGroups*>("orderGroup Unregistered");
  return GPtr->cell(Name,size);
}

} // NAMESPACE ModelSupport
