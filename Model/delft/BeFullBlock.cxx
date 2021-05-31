/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/BeFullBlock.cxx
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
#include "BeFullBlock.h"

namespace delftSystem
{

BeFullBlock::BeFullBlock(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,3)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeFullBlock::BeFullBlock(const BeFullBlock& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  width(A.width),height(A.height),length(A.length),mat(A.mat)
  /*!
    Copy constructor
    \param A :: BeFullBlock to copy
  */
{}

BeFullBlock&
BeFullBlock::operator=(const BeFullBlock& A)
  /*!
    Assignment operator
    \param A :: BeFullBlock to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      width=A.width;
      height=A.height;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}

BeFullBlock::~BeFullBlock() 
 /*!
   Destructor
 */
{}

void
BeFullBlock::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase
 */
{
  ELog::RegMethod RegA("BeFullBlock","populate");

  attachSystem::FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
  
void
BeFullBlock::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param sideIndex :: Side Direction for link point
  */
{
  ELog::RegMethod RegA("BeFullBlock","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
BeFullBlock::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeFullBlock","createSurfaces");

  // Outer layers
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  return;
}

void
BeFullBlock::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeFullBlock","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  
  return;
}

void
BeFullBlock::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("BeFullBlock","createLinks");

  // FixedComp::setConnect(0,Origin,-Y);      
  // FixedComp::setConnect(1,Origin+Y*length,Y);
  // FixedComp::setConnect(2,Origin+X*outerRadius+Y*length/2.0,X);

  // FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));
  // FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  // FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));

  return;
}

void
BeFullBlock::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: Side index for link point 
  */
{
  ELog::RegMethod RegA("BeFullBlock","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}

  
}  // NAMESPACE delftSystem
