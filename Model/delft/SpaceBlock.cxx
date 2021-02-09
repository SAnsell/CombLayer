/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/SpaceBlock.cxx
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceBlock.h"

namespace delftSystem
{

SpaceBlock::SpaceBlock(const std::string& Key,const size_t Index)  :
  attachSystem::FixedOffset(Key+std::to_string(Index),6),
  attachSystem::ContainedComp(),baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: ID number
  */
{}

SpaceBlock::SpaceBlock(const SpaceBlock& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  baseName(A.baseName),
  activeFlag(A.activeFlag),length(A.length),width(A.width),
  height(A.height),mat(A.mat)
  /*!
    Copy constructor
    \param A :: SpaceBlock to copy
  */
{}

SpaceBlock&
SpaceBlock::operator=(const SpaceBlock& A)
  /*!
    Assignment operator
    \param A :: SpaceBlock to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      activeFlag=A.activeFlag;
      length=A.length;
      width=A.width;
      height=A.height;
      mat=A.mat;
    }
  return *this;
}

SpaceBlock::~SpaceBlock() 
 /*!
   Destructor
 */
{}

void
SpaceBlock::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: FuncDatabase
 */
{
  ELog::RegMethod RegA("SpaceBlock","populate");

  activeFlag=Control.EvalDefVar<int>(keyName+"Flag",0);
  if (activeFlag>0)
    {
      attachSystem::FixedOffset::populate(Control);
      length=Control.EvalVar<double>(keyName+"Length");
      width=Control.EvalVar<double>(keyName+"Width");
      height=Control.EvalVar<double>(keyName+"Height");
      mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
    }
  return;
}
  
void
SpaceBlock::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param sideIndex :: link point for origin/axis
  */
{
  ELog::RegMethod RegA("SpaceBlock","createUnitVector");
  
  // PROCESS Origin of a point
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
SpaceBlock::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SpaceBlock","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  return;
}

void
SpaceBlock::createObjects(Simulation& System)
  /*!
    Adds the block components
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("SpaceBlock","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));

  addOuterSurf(Out);

  return;
}

void
SpaceBlock::createLinks()
  /*!
    Create all the links:
  */
{
  ELog::RegMethod RegA("SpaceBlock","createLinks");

  FixedComp::setConnect(0,Origin,-Y);      
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);

  
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
SpaceBlock::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: Side index [0 ==> FC origin]
    \return flag status
  */
{
  ELog::RegMethod RegA("SpaceBlock","createAll");

  populate(System.getDataBase());
  if (activeFlag>0)
    {
      createUnitVector(FC,sideIndex);
      createSurfaces();
      createObjects(System);
      insertObjects(System);       
    }
  return;
}

  
}  // NAMESPACE delftSystem
