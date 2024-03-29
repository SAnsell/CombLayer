/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/UTubePipe.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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

#include "GeneralPipe.h"
#include "UTubePipe.h"

namespace xraySystem
{

UTubePipe::UTubePipe(const std::string& Key) :
  constructSystem::GeneralPipe(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


UTubePipe::~UTubePipe()
  /*!
    Destructor
  */
{}

void
UTubePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("UTubePipe","populate");

  GeneralPipe::populate(Control);

  // Void + Fe special:
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");

  return;
}

void
UTubePipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("UTubePipe","createSurfaces");

  GeneralPipe::createCommonSurfaces();
  GeneralPipe::createFlangeSurfaces();
  
  // main pipe
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);


  // two inner corners
  ModelSupport::buildCylinder
    (SMap,buildIndex+7,Origin-X*(width/2.0),Y,height/2.0);
  ModelSupport::buildCylinder
    (SMap,buildIndex+8,Origin+X*(width/2.0),Y,height/2.0);

  ModelSupport::buildCylinder
    (SMap,buildIndex+17,Origin-X*(width/2.0),Y,height/2.0+pipeThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+18,Origin+X*(width/2.0),Y,height/2.0+pipeThick);

  // main pipe walls
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(pipeThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(pipeThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(pipeThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(pipeThick+height/2.0),Z);



  return;
}

void
UTubePipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum system
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("UTubePipe","createObjects");

  HeadRule HR;

  const HeadRule& frontHR=getFrontRule();
  const HeadRule& backHR=getBackRule();

  // Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3 -7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"4 -8");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 3 -4 6 -16");
  makeCell("TopPipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 3 -4 -5 15");
  makeCell("BasePipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 -3 7 -17 15 -16");
  makeCell("LeftPipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 4 8 -18 15 -16");
  makeCell("RightPipe",System,cellIndex++,pipeMat,0.0,HR);

  
  // FLANGE Front:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107 -3 7");
  makeCell("FrontFlange",System,cellIndex++,pipeMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107 4 8");
  makeCell("FrontFlange",System,cellIndex++,pipeMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107 3 -4 -5");
  makeCell("FrontFlange",System,cellIndex++,pipeMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107 3 -4 6");
  makeCell("FrontFlange",System,cellIndex++,pipeMat,0.0,HR*frontHR);

  // FLANGE Back:

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -207 -3 7");
  makeCell("BackFlange",System,cellIndex++,pipeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -207 4 8");
  makeCell("BackFlange",System,cellIndex++,pipeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -207 3 -4 -5");
  makeCell("BackFlange",System,cellIndex++,pipeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -207 3 -4 6");
  makeCell("BackFlange",System,cellIndex++,pipeMat,0.0,HR*backHR);

  // outer boundary [flange front/back]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107");
  addOuterSurf("FlangeA",HR*frontHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -207");
  addOuterSurf("FlangeB",HR*backHR);
  // outer boundary mid tube
  //  (3:-7) (-4:8) 5 -6 ");
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -201 15 -16 (3:-17) (-4:-18)");
  addOuterSurf("Main",HR);
  return;
}

void
UTubePipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("UTubePipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setConnect(2,Origin-X*(pipeThick+height/2.0+width/2.0),-X);
  FixedComp::setConnect(3,Origin-X*(pipeThick+height/2.0+width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(pipeThick+height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(pipeThick+height/2.0),Z);

  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+18));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}


void
UTubePipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("UTubePipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
