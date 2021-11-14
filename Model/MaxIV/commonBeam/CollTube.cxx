/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/CollTube.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
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
#include "ExternalCut.h"
#include "FrontBackCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "CollTube.h"

namespace xraySystem
{

CollTube::CollTube(const std::string& Key) :
  attachSystem::FixedRotate(Key,10),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


CollTube::~CollTube() 
  /*!
    Destructor
  */
{}

void
CollTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("CollTube","populate");

  FixedRotate::populate(Control);

  mainRadius=Control.EvalVar<double>(keyName+"MainRadius");
  xRadius=Control.EvalVar<double>(keyName+"XRadius");  // x-beam axis
  yRadius=Control.EvalVar<double>(keyName+"YRadius");  // beam axis
  zRadius=Control.EvalVar<double>(keyName+"ZRadius");   // vertical
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  backLength=Control.EvalVar<double>(keyName+"BackLength");

  flangeXRadius=Control.EvalVar<double>(keyName+"FlangeXRadius");
  flangeYRadius=Control.EvalVar<double>(keyName+"FlangeYRadius");
  flangeZRadius=Control.EvalVar<double>(keyName+"FlangeZRadius");

  flangeXLength=Control.EvalVar<double>(keyName+"FlangeXLength");
  flangeYLength=Control.EvalVar<double>(keyName+"FlangeYLength");
  flangeZLength=Control.EvalVar<double>(keyName+"FlangeZLength");

  
  sideZOffset=Control.EvalVar<double>(keyName+"SideZOffset");
  sideLength=Control.EvalVar<double>(keyName+"SideLength");
   
  gaugeZOffset=Control.EvalVar<double>(keyName+"GaugeZOffset");
  gaugeRadius=Control.EvalVar<double>(keyName+"GaugeRadius");
  gaugeLength=Control.EvalVar<double>(keyName+"GaugeLength");
  gaugeHeight=Control.EvalVar<double>(keyName+"GaugeHeight");
  gaugeFlangeRadius=Control.EvalVar<double>(keyName+"GaugeFlangeRadius");
  gaugeFlangeLength=Control.EvalVar<double>(keyName+"GaugeFlangeLength");
  
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");

  return;
}


void
CollTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CollTube","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*frontLength,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*backLength,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // vertical/horizontal dividers
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+300,Origin,Z);

  // front/back pipe and thickness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,mainRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,mainRadius+wallThick);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length/2.0),Y);

  ModelSupport::buildPlane
    (SMap,buildIndex+21,Origin-Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+22,Origin+Y*(wallThick+length/2.0),Y);

  // ports
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,yRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,Y,yRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,Origin,Y,flangeYRadius);
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+101,Y,flangeYLength);
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+102,Y,-flangeYLength);

  // VERTICAL PIPE
  ModelSupport::buildPlane(SMap,buildIndex+405,Origin-Z*depth,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+415,Origin-Z*(depth-flangeZLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+425,Origin-Z*(depth+plateThick),Z);

  ModelSupport::buildPlane(SMap,buildIndex+406,Origin+Z*height,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+416,Origin+Z*(height-flangeZLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+426,Origin+Z*(height+plateThick),Z);
  
  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,Z,zRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+417,Origin,Z,zRadius+wallThick);
  SurfMap::makeCylinder("OuterFlange",SMap,buildIndex+427,Origin,Z,flangeZRadius);

  // RIGHT NORMAL SIDE PIPE
  const Geometry::Vec3D sideOrg(Origin+Z*sideZOffset);
    ModelSupport::buildCylinder(SMap,buildIndex+507,sideOrg,X,xRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+517,sideOrg,X,xRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+527,sideOrg,X,flangeXRadius);

  ModelSupport::buildPlane(SMap,buildIndex+504,sideOrg+X*sideLength,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+514,Origin+X*(sideLength-flangeXLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+524,Origin+X*(sideLength+plateThick),X);



  
  // GAUGE TUBE
  Geometry::Vec3D gaugeOrg(Origin+Z*gaugeZOffset);
  ModelSupport::buildCylinder(SMap,buildIndex+607,gaugeOrg,X,gaugeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+617,gaugeOrg,X,gaugeRadius+wallThick);


  gaugeOrg-=X*gaugeLength;
  // divider
  ModelSupport::buildPlane(SMap,buildIndex+600,gaugeOrg,(-X+Z)/2.0);

  ModelSupport::buildCylinder(SMap,buildIndex+657,gaugeOrg,Z,gaugeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+667,gaugeOrg,Z,gaugeRadius+wallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+677,gaugeOrg,Z,gaugeFlangeRadius);
  
  ModelSupport::buildPlane(SMap,buildIndex+606,gaugeOrg+Z*gaugeLength,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+616,gaugeOrg+Z*(gaugeLength-gaugeFlangeLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+626,gaugeOrg+Z*(gaugeLength+plateThick),Z);
  // outer plane
  ModelSupport::buildPlane
    (SMap,buildIndex+693,gaugeOrg-X*(gaugeFlangeRadius+Geometry::zeroTol),X);
  
  return;
}

void
CollTube::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CollTube","createObjects");

  HeadRule HR;
  
  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 -7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -107");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -107");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(-100:507) (100:607) 107 417 (-11:12:7) 21 -22 -17");
  makeCell("Main",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-21 107 -117");
  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 117 -127");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 117 -127 -21");
  makeCell("FrontOuter",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"22 107 -117");
  makeCell("BackWall",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 117 -127");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-102 117 -127 22");
  makeCell("BackOuter",System,cellIndex++,0,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -407 405 -406");
  makeCell("PipeVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -417 407 405 -406");
  makeCell("PipeWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-427 406 -426");
  makeCell("TopPlate",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-427 417 -406 416");
  makeCell("TopFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "(100:617) -427 417 -416 415 17 (-100 :527)");
  makeCell("VerticalVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-427 -405 425");
  makeCell("BasePlate",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-427 417 405 -415");
  makeCell("BaseFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 7 -507 -504 ");
  makeCell("RightVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 17 -517 507 -504 ");
  makeCell("RightWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"517 -527 -504 514");
  makeCell("RightFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-527 -524 504");
  makeCell("RightPlate",System,cellIndex++,plateMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 17 517 -527 -514 417");
  makeCell("RightOuter",System,cellIndex++,0,0.0,HR);

  // Gauge tube
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 7 -607 -600");
  makeCell("GaugeVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 17 607 -617 -600 ");
  makeCell("GaugeWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-606 -657 600");
  makeCell("GaugeVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-606 657 -667 600 ");
  makeCell("GaugeVWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-606 667 -677 616");
  makeCell("GaugeFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-626 -677 606");
  makeCell("GaugePlate",System,cellIndex++,wallMat,0.0,HR);

  // cut at base of GaugeFlange
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"17 427 21 -22 693 -524 425 -616 (527:-100)");
  HeadRule HRunit=ModelSupport::getHeadRule(SMap,buildIndex,"-100 -617 -600");
  HR*=HRunit.complement();
  HRunit=ModelSupport::getHeadRule(SMap,buildIndex,"-606 -667 600");
  HR*=HRunit.complement();
  makeCell("BlockVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"427 21 -22 693 -524 425 616 -426 (626:677)");
  makeCell("TopBlockVoid",System,cellIndex++,0,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-127");
  addOuterUnionSurf(HR*frontHR*backHR);
    
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"21 -22 693 -524 425 -426");
  addOuterUnionSurf(HR);
  return;
}

void 
CollTube::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("CollTube","createLinks");

  nameSideIndex(2,"OuterRadius");
  
  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  FixedComp::setConnect(2,Origin-X*(mainRadius+wallThick),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+417));

  FixedComp::setConnect(3,Origin+X*(mainRadius+wallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+417));

  FixedComp::setConnect(4,Origin-Z*(depth+plateThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+425));

  FixedComp::setConnect(5,Origin+Z*(height+plateThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+426));

  // point on vertical cylinder [-ve]
  FixedComp::setConnect(6,Origin+Y*(zRadius+wallThick)-
			Z*((mainRadius+depth)/2.0),Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+417));

  // point on vertical cylinder [+ve]
  FixedComp::setConnect(7,Origin+Y*(zRadius+wallThick)+
			Z*((mainRadius+height)/2.0),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+417));


  // point on vertical cylinder [+ve]
  FixedComp::setConnect(8,Origin-Z*((mainRadius+depth)/2.0),Z);

  FixedComp::nameSideIndex(6,"VertOuterWall");
  FixedComp::nameSideIndex(8,"VertCentre");

  return;
}

void
CollTube::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CollTube","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,frontLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
