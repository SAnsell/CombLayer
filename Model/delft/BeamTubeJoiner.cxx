/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/BeamTubeJoiner.cxx
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
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BeamTubeJoiner.h"

namespace delftSystem
{

BeamTubeJoiner::BeamTubeJoiner(const std::string& Key)  :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeamTubeJoiner::BeamTubeJoiner(const BeamTubeJoiner& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  innerStep(A.innerStep),
  length(A.length),innerRadius(A.innerRadius),innerWall(A.innerWall),
  outerRadius(A.outerRadius),outerWall(A.outerWall),
  interThick(A.interThick),interFrontWall(A.interFrontWall),
  interWallThick(A.interWallThick),interYOffset(A.interYOffset),
  interMat(A.interMat),wallMat(A.wallMat),gapMat(A.gapMat)
  /*!
    Copy constructor
    \param A :: BeamTubeJoiner to copy
  */
{}

BeamTubeJoiner&
BeamTubeJoiner::operator=(const BeamTubeJoiner& A)
  /*!
    Assignment operator
    \param A :: BeamTubeJoiner to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      innerStep=A.innerStep;
      length=A.length;
      innerRadius=A.innerRadius;
      innerWall=A.innerWall;
      outerRadius=A.outerRadius;
      outerWall=A.outerWall;
      interThick=A.interThick;
      interFrontWall=A.interFrontWall;
      interWallThick=A.interWallThick;
      interYOffset=A.interYOffset;
      interMat=A.interMat;
      wallMat=A.wallMat;
      gapMat=A.gapMat;
    }
  return *this;
}

  
BeamTubeJoiner::~BeamTubeJoiner() 
 /*!
   Destructor
 */
{}

void
BeamTubeJoiner::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to used
 */
{
  ELog::RegMethod RegA("BeamTubeJoiner","populate");

  attachSystem::FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerWall=Control.EvalVar<double>(keyName+"InnerWall");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerWall=Control.EvalVar<double>(keyName+"OuterWall");

  interThick=Control.EvalVar<double>(keyName+"InterThick");
  interWallThick=Control.EvalVar<double>(keyName+"InterWallThick");
  interYOffset=Control.EvalVar<double>(keyName+"InterYOffset");
  interFrontWall=Control.EvalVar<double>(keyName+"InterFrontWall");
  interMat=ModelSupport::EvalMat<int>(Control,keyName+"InterMat");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  gapMat=ModelSupport::EvalMat<int>(Control,keyName+"GapMat");

  return;
}
  
void
BeamTubeJoiner::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamTubeJoiner","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+7,
			      Origin,Y,outerRadius+outerWall);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);


  // Outer Wall

  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,outerRadius);

  // Gap layer
  ModelSupport::buildCylinder(SMap,buildIndex+27,
			      Origin,Y,innerRadius+innerWall);

  // Gap layer
  ModelSupport::buildCylinder(SMap,buildIndex+37,
			      Origin,Y,innerRadius);

  ModelSupport::buildPlane(SMap,buildIndex+41,
			   Origin+Y*interYOffset,Y);
  ModelSupport::buildPlane(SMap,buildIndex+51,
			   Origin+Y*(interYOffset+interFrontWall),Y);

  // Inter layer
  // Layers [wall : Mid : Outer ]
  ModelSupport::buildCylinder
    (SMap,buildIndex+47,Origin,Y,outerRadius-interWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+57,Origin,Y,outerRadius-(interWallThick+interThick));
  ModelSupport::buildCylinder
    (SMap,buildIndex+67,Origin,Y,outerRadius-(2.0*interWallThick+interThick));

  return;
}


void
BeamTubeJoiner::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
    \param FC :: FixedComp to start join from
    \param sideIndex :: link point index
  */
{
  ELog::RegMethod RegA("BeamTubeJoiner","createObjects");

  const HeadRule frontHR=getRule("front");

  HeadRule HR;
  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -7 17");
  System.addCell(cellIndex++,wallMat,0.0,HR*frontHR);

  // Void (exclude inter wall)
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -17 27 (-41:-67)");
  makeCell("FrontVoid",System,cellIndex++,gapMat,0.0,HR*frontHR);
  
  // Inner wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -27 37");
  System.addCell(cellIndex++,wallMat,0.0,HR*frontHR);
  
  // Inner Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -37");  
  makeCell("Void",System,cellIndex++,0,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-47 57 51 -2");
  System.addCell(cellIndex++,interMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				"41 -17 67 (47:-57:-51) -2");
  System.addCell(cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -7");
  addOuterSurf(HR*frontHR);
  
  return;
}

void
BeamTubeJoiner::createLinks(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create All the links:
    \param FC :: FixedComp for the start surf
    \param sideIndex :: link point for side point
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("BeamTubeJoiner","createLinks");

  FixedComp::setLinkCopy(0,FC,sideIndex);
  setConnect(1,Origin+Y*length,Y);
  setLinkSurf(1,SMap.realSurf(buildIndex+2));
  return;
}

void
BeamTubeJoiner::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Front face object
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("BeamTubeJoiner","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks(FC,sideIndex);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
