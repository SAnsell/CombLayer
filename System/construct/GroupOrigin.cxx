/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/GroupOrigin.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "GroupOrigin.h"

namespace constructSystem
{

GroupOrigin::GroupOrigin(const std::string& Key)  :
  attachSystem::FixedOffset(Key,0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

GroupOrigin::GroupOrigin(const GroupOrigin& A) : 
  attachSystem::FixedOffset(A)
  /*!
    Copy constructor
    \param A :: GroupOrigin to copy
  */
{}

GroupOrigin&
GroupOrigin::operator=(const GroupOrigin& A)
  /*!
    Assignment operator
    \param A :: GroupOrigin to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
    }
  return *this;
}


GroupOrigin::~GroupOrigin() 
 /*!
   Destructor
 */
{}

  

void
GroupOrigin::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
		      
  /*!
    Global creation of the hutch
    \param Control :: Variable DataBase
    \param FC :: Fixed Component to place object within
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("GroupOrigin","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  return;
}

  
}  // NAMESPACE ts1System
