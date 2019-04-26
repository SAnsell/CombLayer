/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: cosaxs/cosaxsExptLine.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell / Konstantin Batkov
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "InnerZone.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "cosaxsExptLine.h"

namespace xraySystem
{

// Note currently uncopied:
  
cosaxsExptLine::cosaxsExptLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
}
  
cosaxsExptLine::~cosaxsExptLine()
  /*!
    Destructor
   */
{}

void
cosaxsExptLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
   */
{
  ELog::RegMethod RegA("cosaxsExptLine","populate");

  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",outerLeft);
  outerTop=Control.EvalDefVar<double>(keyName+"OuterTop",outerLeft);
  
  return;
}

void
cosaxsExptLine::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    Note that the FC:in and FC:out are tied to Main
    -- rotate position Main and then Out/In are moved relative

    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("cosaxsExptLine","createUnitVector");

  FixedOffset::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
cosaxsExptLine::createSurfaces()
  /*!
    Create surfaces for outer void
  */
{
  ELog::RegMethod RegA("cosaxsExptLine","createSurface");

  ELog::EM << "Remove surfaces?" << ELog::endCrit;
  ELog::EM << "Is buildZone needed here?" << ELog::endCrit;
  if (outerLeft>Geometry::zeroTol &&  isActive("floor"))
    {
      std::string Out;
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*outerTop,Z);
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6");
      const HeadRule HR(Out+getRuleStr("floor"));
      buildZone.setSurround(HR);
    }

  return;
}

void
cosaxsExptLine::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("cosaxsExptLine","buildObjects");


  //  setCell("LastVoid",masterCell->getName());

  //  lastComp=gateH;

  return;
}

void
cosaxsExptLine::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RControl("cosaxsExptLine","createLinks");
  
  //  setLinkSignedCopy(0,*pipeInit,1);
  //  setLinkSignedCopy(1,*lastComp,2);
  return;
}
  
  
void 
cosaxsExptLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RControl("cosaxsExptLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE xraySystem

