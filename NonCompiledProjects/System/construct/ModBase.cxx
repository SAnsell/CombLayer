/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ModBase.cxx
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

#include "Exception.h"
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
#include "ModelSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ModBase.h"

namespace constructSystem
{

ModBase::ModBase(const std::string& Key,const size_t nLinks)  :
  attachSystem::ContainedComp(),attachSystem::LayerComp(0,0),
  attachSystem::FixedOffset(Key,nLinks),attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param nLinks :: Number of links
  */
{}

ModBase::ModBase(const ModBase& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A)
  /*!
    Copy constructor
    \param A :: ModBase to copy
  */
{}

ModBase&
ModBase::operator=(const ModBase& A)
  /*!
    Assignment operator
    \param A :: ModBase to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
    }
  return *this;
}


ModBase::~ModBase() 
 /*!
   Destructor
 */
{}


void
ModBase::populate(const FuncDataBase& Control)
  /*!
    Local populations
    \param Control :: FuncDatabase to use
   */
{
  ELog::RegMethod RegA("ModBase","populate");

  FixedOffset::populate(Control);
  
  const size_t NFlight=Control.EvalDefVar<size_t>(keyName+"NFlight",0);  
  flightSides.clear();
  for(size_t i=0;i<NFlight;i++)
    {
      const std::string kName=keyName+"FlightSide"+std::to_string(i);
      const long int I=Control.EvalVar<long int>(kName);  
      flightSides.push_back(I);
    }

  return;
}

void
ModBase::createUnitVector(const attachSystem::FixedComp& orgFC,
			  const long int orgIndex,
			  const attachSystem::FixedComp& axisFC,
			  const long int axisIndex)
  /*!
    Local create unit for central Base.
    \param axisFC :: Axis coordinate system
    \param orgFC :: if given origin point
    \param orgIndex :: link point of orgFC
    \param axisIndex :: link point of orgFC
  */
{
  ELog::RegMethod RegA("ModBase","createUnitVector");
  
  attachSystem::FixedComp::createUnitVector(axisFC,axisIndex);
  
  Origin= orgFC.getLinkPt(orgIndex);
  FixedOffset::applyOffset();
  return; 
}

void
ModBase::createUnitVector(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Local create unit for central Base.
    \param axisFC :: Axis coordinate system
    \param orgFC :: if given origin point
    \param sideIndex :: link point of orgFC
  */
{
  ELog::RegMethod RegA("ModBase","createUnitVector");
  
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  FixedOffset::applyOffset();
  return; 
}

const attachSystem::FixedComp&
ModBase::getComponent(const std::string& compName) const
  /*!
    Simple way to get a named component of this object
    \param compName :: Component name
    \return FixedComp object
  */
{
  ELog::RegMethod RegA("ModBase","getComponent");

  throw ColErr::InContainerError<std::string>(compName,keyName+" component");
}

  
std::string
ModBase::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,buildIndex,surfList);
}


long int 
ModBase::getSideIndex(const size_t I) const
  /*!
    Accessor to flight sides
    \param I :: Index for side
    \return flight surface index 
  */
{
  return (I>=flightSides.size()) ? 0 : flightSides[I];
}

void
ModBase::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to get axis / origin
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("ModBase","createAll");
  // virtual call
  this->createAll(System,FC,sideIndex,FC,0);
  return;
}

  
  
}  // NAMESPACE constructSystem
