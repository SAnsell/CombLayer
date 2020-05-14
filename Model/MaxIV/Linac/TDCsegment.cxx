/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/TDCsegment.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "InnerZone.h"

#include "TDCsegment.h"

namespace tdcSystem
{

// Note currently uncopied:
  
TDCsegment::TDCsegment(const std::string& Key,const size_t NL) :
  attachSystem::FixedRotate(Key,NL),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(nullptr),
  lastFlag(0)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param NL :: number of links
  */
{}
  
TDCsegment::~TDCsegment()
  /*!
    Destructor
   */
{}

void
TDCsegment::setLastSurf(const HeadRule& HR)
  /*!
    Set the last surface rule
    \param HR :: Head rule to use
   */
{
  lastFlag=1;
  lastRule=HR;
  return;
}


}   // NAMESPACE tdcSystem

