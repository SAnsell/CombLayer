/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/DiffPump.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
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
#include "SurfMap.h"
#include "ExternalCut.h"

#include "DiffPump.h"

namespace xraySystem
{

DiffPump::DiffPump(const std::string& Key) :
  attachSystem::ContainedGroup("Main","PortA","PortB"),
  attachSystem::FixedRotate(Key,8),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

DiffPump::~DiffPump()
  /*!
    Destructor
  */
{}

void
DiffPump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("DiffPump","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");

  innerLength=Control.EvalVar<double>(keyName+"InnerLength");

  captureWidth=Control.EvalVar<double>(keyName+"CaptureWidth");
  captureHeight=Control.EvalVar<double>(keyName+"CaptureHeight");

  magWidth=Control.EvalVar<double>(keyName+"MagWidth");
  magHeight=Control.EvalVar<double>(keyName+"MagHeight");

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerThick=Control.EvalVar<double>(keyName+"InnerThick");

  portLength=Control.EvalVar<double>(keyName+"PortLength");
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portThick=Control.EvalVar<double>(keyName+"PortThick");
  
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  magnetMat=ModelSupport::EvalMat<int>(Control,keyName+"MagnetMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
DiffPump::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DiffPump","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1,Origin-Y*(portLength+length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // main 
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+103,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+104,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+105,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+106,Origin+Z*(height/2.0),Z);

  // magnet (outer surf)
  ModelSupport::buildPlane
    (SMap,buildIndex+203,Origin-X*(magWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+204,Origin+X*(magWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+205,Origin-Z*(magHeight/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+206,Origin+Z*(magHeight/2.0),Z);

  // capture
  ModelSupport::buildPlane
    (SMap,buildIndex+303,Origin-X*(captureWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+304,Origin+X*(captureWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+305,Origin-Z*(captureHeight/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+306,Origin+Z*(captureHeight/2.0),Z);


  // Inner tube
  ModelSupport::buildCylinder
    (SMap,buildIndex+407,Origin,Y,innerRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+417,Origin,Y,innerRadius+innerThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+427,Origin,Y,flangeRadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+411,Origin-Y*(length/2.0+portLength-flangeLength),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+412,Origin+Y*(length/2.0+portLength-flangeLength),Y);  
  
  return;
}

void
DiffPump::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DiffPump","createObjects");

  HeadRule HR;
  const HeadRule frontHR(getRule("front"));
  const HeadRule backHR(getRule("back"));

  // Main pipe

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -407");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 407 -417");
  makeCell("Pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 417 303 -304 305 -306");
  makeCell("captureVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 203 -204 205 -206 (-303:304:-305:306)");
  makeCell("Magnet",System,cellIndex++,magnetMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 103 -104 105 -106 (-203:204:-205:206)");
  makeCell("Outer",System,cellIndex++,mainMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," -407 -101 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," -407 102 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-417 407 -101 ");
  makeCell("PortTube",System,cellIndex++,pipeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-417 407 102 ");
  makeCell("PortTube",System,cellIndex++,pipeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-427 417 -411");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-427 417 412");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-427 417 -101 411");
  makeCell("PortVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-427 417 -102 412");
  makeCell("PortVoid",System,cellIndex++,voidMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 103 -104 105 -106");
  addOuterSurf("Main",HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -427");
  addOuterSurf("PortA",HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -427");
  addOuterSurf("PortB",HR*backHR);
  
  return;
}


void
DiffPump::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("DiffPump","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  
  return;
}

void
DiffPump::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Si\mulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DiffPump","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // constructSystem
