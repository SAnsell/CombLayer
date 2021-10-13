/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/DCMTank.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "portItem.h"

#include "DCMTank.h"

namespace xraySystem
{

DCMTank::DCMTank(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  centreOrigin(0),delayPortBuild(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
    \param flag :: true if the box is centred on coordinates
  */
{}

DCMTank::~DCMTank() 
  /*!
    Destructor
  */
{}

void
DCMTank::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("DCMTank","populate");
  
  FixedRotate::populate(Control);

  // Void + Fe special:
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");

  topLift=Control.EvalVar<double>(keyName+"TopLift");
  
  portAXStep=Control.EvalDefVar<double>(keyName+"PortAXStep",0.0);
  portAZStep=Control.EvalDefVar<double>(keyName+"PortAZStep",0.0);
  portAWallThick=Control.EvalPair<double>(keyName+"PortAWallThick",
					  keyName+"PortWallThick");
  portATubeLength=Control.EvalPair<double>(keyName+"PortATubeLength",
					   keyName+"PortTubeLength");
  portATubeRadius=Control.EvalPair<double>(keyName+"PortATubeRadius",
					   keyName+"PortTubeRadius");

  portBXStep=Control.EvalDefVar<double>(keyName+"PortBXStep",0.0);
  portBZStep=Control.EvalDefVar<double>(keyName+"PortBZStep",0.0);

  portBWallThick=Control.EvalPair<double>(keyName+"PortBWallThick",
					  keyName+"PortWallThick");
  portBTubeLength=Control.EvalPair<double>(keyName+"PortBTubeLength",
					   keyName+"PortTubeLength");
  portBTubeRadius=Control.EvalPair<double>(keyName+"PortBTubeRadius",
					   keyName+"PortTubeRadius");

  
  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");
  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
					 keyName+"FlangeLength");
  flangeAXStep=Control.EvalDefPair<double>(keyName+"FlangeAXStep",
					   keyName+"FlangeXStep",0.0);
  flangeAZStep=Control.EvalDefPair<double>(keyName+"FlangeAZStep",
					   keyName+"FlangeZStep",0.0);
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");
  flangeBXStep=Control.EvalDefPair<double>(keyName+"FlangeBXStep",
					   keyName+"FlangeXStep",0.0);
  flangeBZStep=Control.EvalDefPair<double>(keyName+"FlangeBZStep",
					   keyName+"FlangeZStep",0.0);

  if (flangeARadius<(portAWallThick+portATubeRadius+Geometry::zeroTol))
    throw ColErr::SizeError<double>
      (flangeARadius,portAWallThick+portATubeRadius,
       "Flange to small for "+keyName+" port A");

  if (flangeBRadius<(portBWallThick+portBTubeRadius+Geometry::zeroTol))
    throw ColErr::SizeError<double>
      (flangeBRadius,portBWallThick+portBTubeRadius,
       "Flange to small for "+keyName+" port B");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  const size_t NPorts=Control.EvalVar<size_t>(keyName+"NPorts");
  const std::string portBase=keyName+"Port";
  for(size_t i=0;i<NPorts;i++)
    {
      const std::string portName=portBase+std::to_string(i);
      constructSystem::portItem windowPort(portBase,portName);
      windowPort.populate(Control);
      
      const Geometry::Vec3D Centre=
	Control.EvalVar<Geometry::Vec3D>(portName+"Centre");
      const Geometry::Vec3D Axis=
	Control.EvalTail<Geometry::Vec3D>(portName,portBase,"Axis");

      PCentre.push_back(Centre);
      PAxis.push_back(Axis);
      Ports.push_back(windowPort);
    }					    

  outerSize=Control.EvalDefVar<double>(keyName+"OuterSize",voidRadius+20.0);

  return;
}

void
DCMTank::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("DCMTank","createSurfaces");
  const double topRadius=
    (voidRadius*voidRadius+topLift*topLift)/(2.0*topLift);

  // Do outer surfaces (vacuum ports)
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
	      Origin-Y*(portATubeLength+wallThick+voidRadius),Y);
      setCutSurf("front",SMap.realSurf(buildIndex+101));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+102,
	    Origin+Y*(portBTubeLength+wallThick+voidRadius),Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+102));
    }

  // OUTER VOID
  // mid-line
  const double maxPortWidth
    (std::max(std::abs(portAXStep)+flangeARadius,
	      std::abs(portBXStep)+flangeBRadius)+0.01);
  const double maxPortHeight
    (std::max(std::abs(portAZStep)+flangeARadius,
	      std::abs(portBZStep)+flangeBRadius)+0.01);
  // mid layer divider
  ModelSupport::buildPlane(SMap,buildIndex+1000,Origin,Y);
  
  ModelSupport::buildPlane(SMap,buildIndex+1003,Origin-X*outerSize,X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+X*outerSize,X);  
  ModelSupport::buildPlane(SMap,buildIndex+1005,Origin-Z*outerSize,Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,Origin+Z*outerSize,Z);  

  ModelSupport::buildPlane(SMap,buildIndex+1013,Origin-X*maxPortWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+1014,Origin+X*maxPortWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+1015,Origin+Z*maxPortHeight,Z);
  
  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);  

  makeCylinder("innerCylinder",SMap,buildIndex+7,Origin,Z,voidRadius);
  setCutSurf("innerRadius",-SMap.realSurf(buildIndex+7));
  makeCylinder("outerCylinder",SMap,buildIndex+17,Origin,Z,voidRadius+wallThick);
    
  // Top plate
  const Geometry::Vec3D topCent=Origin-Z*(topRadius-(voidHeight+topLift));
  ModelSupport::buildSphere(SMap,buildIndex+108,topCent,topRadius);
  ModelSupport::buildSphere(SMap,buildIndex+118,topCent,topRadius+wallThick);

  // base plate

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-X*(baseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+X*(baseWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-Y*(baseLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+Y*(baseLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(voidDepth+baseThick),Z);  
  
  // FRONT PORT
  const Geometry::Vec3D ACentre(Origin+X*portAXStep+Z*portAZStep);
  const Geometry::Vec3D AFCentre(ACentre+X*flangeAXStep+Z*flangeAZStep);
  ModelSupport::buildCylinder(SMap,buildIndex+507,ACentre,Y,portATubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+517,ACentre,Y,
			      portATubeRadius+portAWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+527,AFCentre,Y,flangeARadius);


  // BACK PORT
  const Geometry::Vec3D BCentre(Origin+X*portBXStep+Z*portBZStep);
  ModelSupport::buildCylinder(SMap,buildIndex+607,BCentre,Y,portBTubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+617,BCentre,Y,
			      portBTubeRadius+portBWallThick);
  const Geometry::Vec3D BFCentre(BCentre+X*flangeBXStep+Z*flangeBZStep);
  ModelSupport::buildCylinder(SMap,buildIndex+627,BFCentre,Y,flangeBRadius);

  // Flange cut
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+511,Y,flangeALength);
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+611,Y,-flangeBLength);

  return;
}

void
DCMTank::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("DCMTank","createObjects");

  std::string Out;

  const std::string FPortStr(ExternalCut::getRuleStr("front"));
  const std::string BPortStr(ExternalCut::getRuleStr("back"));
  
  // Main Void 
  Out=ModelSupport::getComposite(SMap,buildIndex,"  5 (-6:-108) -7 ");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -17 6 -118 108 (507:1000) (607:-1000) ");
  CellMap::makeCell("TopPlate",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "  5 (-6:-108) 7 -17 (507:1000) (607:-1000)");
  CellMap::makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14  -5 15 ");
  CellMap::makeCell("BasePlate",System,cellIndex++,wallMat,0.0,Out);

  // Front/Back port

  // PORTS:
  // front port
  Out=ModelSupport::getComposite(SMap,buildIndex," -1000 (7:108) -507 ");
  CellMap::makeCell("PortAVoid",System,cellIndex++,voidMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1000 (17:118) 507 -517 ");
  CellMap::makeCell("PortAWall",System,cellIndex++,wallMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -511 517 -527 ");
  CellMap::makeCell("PortAFlange",System,cellIndex++,wallMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1000 (17:118) 511 517 -527 ");
  CellMap::makeCell("PortAScreen",System,cellIndex++,voidMat,0.0,Out);

  // back port
  Out=ModelSupport::getComposite(SMap,buildIndex," 1000 (7:108) -607 ");
  CellMap::makeCell("PortBVoid",System,cellIndex++,voidMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1000 (17:118) 607 -617 ");
  CellMap::makeCell("PortBWall",System,cellIndex++,wallMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 611 617 -627 ");
  CellMap::makeCell("PortBFlange",System,cellIndex++,wallMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1000 (17:118) -611 617 -627 ");
  CellMap::makeCell("PortBScreen",System,cellIndex++,voidMat,0.0,Out);

  // OUTER VOID SPACE
  const std::string fbCut=FPortStr+BPortStr;
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 527 1013 -1014 5 -1015 (17:118) -1000 ");
  CellMap::makeCell("OuterFrontVoid",System,cellIndex++,0,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 627 1013 -1014 5 -1015 (17:118) 1000");
  CellMap::makeCell("OuterBackVoid",System,cellIndex++,0,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1003 -1013 5 -1015 (17:118) ");
  CellMap::makeCell("OuterLeftVoid",System,cellIndex++,0,0.0,Out+fbCut);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1004 1014 5 -1015 (17:118) ");
  CellMap::makeCell("OuterRightVoid",System,cellIndex++,0,0.0,Out+fbCut);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1003 -1004 118 1015 -1006 ");
  CellMap::makeCell("OuterTopVoid",System,cellIndex++,0,0.0,Out+fbCut);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1003 -1004 -5 15 (-11:12:-13:14) ");
  CellMap::makeCell("OuterBasePlateVoid",System,cellIndex++,0,0.0,Out+fbCut);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1003 -1004 -15  1005 ");
  CellMap::makeCell("OuterBaseVoid",System,cellIndex++,0,0.0,Out+fbCut);

  
  
  // Main exclusion box
  Out=ModelSupport::getComposite(SMap,buildIndex," 1003 -1004 1005 -1006 ");

  addOuterSurf(Out+fbCut);

  return;
}

void
DCMTank::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("DCMTank","createLinks");

  const Geometry::Vec3D ACentre(Origin+X*portAXStep+Z*portAZStep);
  const Geometry::Vec3D BCentre(Origin+X*portBXStep+Z*portBZStep);

  ExternalCut::createLink("front",*this,0,ACentre,-Y);
  ExternalCut::createLink("back",*this,1,BCentre,Y);

  return;
}

void
DCMTank::createPorts(Simulation& System)
  /*!
    Simple function to create ports
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("DCMTank","createPorts");

  MonteCarlo::Object* wallObject=
    CellMap::getCellObject(System,"Wall");
  
  const HeadRule innerWall=SurfMap::getSurfRule("innerCylinder");
  const HeadRule outerWall=SurfMap::getSurfRule("outerCylinder");

  for(size_t i=0;i<Ports.size();i++)
    {
      for(const int CN : portCells)
	Ports[i].addOuterCell(CN);
      Ports[i].addInsertCell(CellMap::getCell("OuterRightVoid"));
      Ports[i].setCentLine(*this,PCentre[i],PAxis[i]);
      Ports[i].constructTrack(System,wallObject,innerWall,outerWall);
      Ports[i].insertObjects(System);
    }
  return;
}


void
DCMTank::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("DCMTank","createAll(FC)");

  
  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,portATubeLength+wallThick+voidRadius);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  if (!delayPortBuild)
    createPorts(System);
  
  return;
}
  
}  // NAMESPACE xraySystem
