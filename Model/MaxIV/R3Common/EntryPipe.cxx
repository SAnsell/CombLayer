/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3Common/EntryPipe.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "surfDivide.h"

#include "EntryPipe.h"

namespace xraySystem
{

EntryPipe::EntryPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedGroup("Main","Pipe","Flange"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
}


EntryPipe::~EntryPipe()
  /*!
    Destructor
  */
{}

void
EntryPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("EntryPipe","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");


  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalDefMat(Control,keyName+"FlangeMat",wallMat);

  return;
}



void
EntryPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("EntryPipe","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // Front Inner void
  //  getShiftedFront(SMap,buildIndex+11,-Y,flangeLength);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*flangeLength,Y);
      
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  makeCylinder("OuterRadius",SMap,buildIndex+17,Origin,Y,radius+wallThick);
  makeCylinder("FlangeRadius",SMap,buildIndex+27,Origin,Y,flangeRadius);
  return;
}

void
EntryPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("EntryPipe","createObjects");

  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  HeadRule HR;
  // Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 7");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*frontHR*backHR);

  // Front Flange
  HR=ModelSupport::getHeadRule
  (SMap,buildIndex," -27 17 -11 ");
  makeCell("Flange",System,cellIndex++,flangeMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17");
  addOuterSurf("Main",HR*frontHR*backHR);
  addOuterSurf("Pipe",HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27 -11");
  addOuterSurf("Flange",HR*frontHR);

  return;
}


void
EntryPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("EntryPipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back


  return;
}

void
EntryPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("EntryPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
