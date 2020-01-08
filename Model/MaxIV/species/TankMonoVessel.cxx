/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   species/TankMonoVessel.cxx
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
#include <memory>
#include <array>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "portItem.h"

#include "TankMonoVessel.h"

namespace xraySystem
{

TankMonoVessel::TankMonoVessel(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  centreOrigin(0),delayPortBuild(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
    \param flag :: true if the box is centred on coordinates
  */
{}

TankMonoVessel::~TankMonoVessel()
  /*!
    Destructor
  */
{}

void
TankMonoVessel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("TankMonoVessel","populate");

  FixedOffset::populate(Control);

  // Void + Fe special:
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  baseGap=Control.EvalVar<double>(keyName+"BaseGap");
  topGap=Control.EvalVar<double>(keyName+"TopGap");

  lidOffset=Control.EvalVar<double>(keyName+"LidOffset");
  lidRadius=Control.EvalVar<double>(keyName+"LidRadius");
  lidDepth=Control.EvalVar<double>(keyName+"LidDepth");


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

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");


  const size_t NPorts=Control.EvalVar<size_t>(keyName+"NPorts");
  const std::string portBase=keyName+"Port";
  double L,R,W,FR,FT,CT;
  int capMat;
  int OFlag;
  for(size_t i=0;i<NPorts;i++)
    {
      const std::string portName=portBase+std::to_string(i);
      constructSystem::portItem windowPort(portName);
      const Geometry::Vec3D Centre=
	Control.EvalVar<Geometry::Vec3D>(portName+"Centre");
      const Geometry::Vec3D Axis=
	Control.EvalTail<Geometry::Vec3D>(portName,portBase,"Axis");

      L=Control.EvalTail<double>(portName,portBase,"Length");
      R=Control.EvalTail<double>(portName,portBase,"Radius");
      W=Control.EvalTail<double>(portName,portBase,"Wall");
      FR=Control.EvalTail<double>(portName,portBase,"FlangeRadius");
      FT=Control.EvalTail<double>(portName,portBase,"FlangeLength");
      CT=Control.EvalDefTail<double>(portName,portBase,"CapThick",0.0);
      capMat=ModelSupport::EvalDefMat<int>
	(Control,portName+"CapMat",portBase+"CapMat",wallMat);

      OFlag=Control.EvalDefVar<int>(portName+"OuterVoid",0);

      if (OFlag) windowPort.setWrapVolume();
      windowPort.setMain(L,R,W);
      windowPort.setFlange(FR,FT);
      windowPort.setCoverPlate(CT,capMat);
      windowPort.setMaterial(voidMat,wallMat);

      PCentre.push_back(Centre);
      PAxis.push_back(Axis);
      Ports.push_back(windowPort);
    }



  outerSize=Control.EvalDefVar<double>(keyName+"OuterSize",voidRadius+20.0);


  return;
}

void
TankMonoVessel::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("TankMonoVessel","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  Origin+=Y*(portATubeLength+wallThick+voidRadius);
  return;
}

void
TankMonoVessel::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("TankMonoVessel","createSurfaces");

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
	      std::abs(portBXStep)+flangeBRadius));
  // mid layer divider
  ModelSupport::buildPlane(SMap,buildIndex+1000,Origin,Y);

  ModelSupport::buildPlane(SMap,buildIndex+1003,Origin-X*outerSize,X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+X*outerSize,X);
  ModelSupport::buildPlane(SMap,buildIndex+1005,Origin-Z*outerSize,Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,Origin+Z*outerSize,Z);

  ModelSupport::buildPlane(SMap,buildIndex+1013,Origin-X*maxPortWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+1014,Origin+X*maxPortWidth,X);


  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,voidRadius);
  setCutSurf("innerRadius",-SMap.realSurf(buildIndex+7));
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,voidRadius+wallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,lidRadius);
  ModelSupport::buildPlane(SMap,buildIndex+25,
			      Origin+Z*(voidHeight-lidOffset-lidDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			      Origin+Z*(voidHeight-lidOffset),Z);

  // Top plate
  const double xPlus=
    (voidRadius*voidRadius-topGap*topGap)/(2.0*topGap);
  const double topRadius=
    (voidRadius*voidRadius+topGap*topGap)/(2.0*topGap);
  const Geometry::Vec3D topCent=Origin-Z*(xPlus-voidHeight);
  ModelSupport::buildSphere(SMap,buildIndex+108,topCent,topRadius);
  ModelSupport::buildSphere(SMap,buildIndex+118,topCent,topRadius+wallThick);

  // Base plate
  const double xDrop=
    (voidRadius*voidRadius-baseGap*baseGap)/(2.0*baseGap);
  const double baseRadius=
    (voidRadius*voidRadius+baseGap*baseGap)/(2.0*baseGap);
  const Geometry::Vec3D baseCent=Origin+Z*(xDrop-voidHeight);
  ModelSupport::buildSphere(SMap,buildIndex+208,baseCent,baseRadius);
  ModelSupport::buildSphere(SMap,buildIndex+218,baseCent,baseRadius+wallThick);

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
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+511,1,Y,flangeALength);
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+611,-1,Y,flangeBLength);

  return;
}

void
TankMonoVessel::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("TankMonoVessel","createObjects");

  std::string Out;

  const std::string FPortStr(ExternalCut::getRuleStr("front"));
  const std::string BPortStr(ExternalCut::getRuleStr("back"));


  // Main Void
  Out=ModelSupport::getComposite(SMap,buildIndex," (5:-208) (-6:-108) -7 ");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex,"  -17 6 -118 108 ");
  CellMap::makeCell("TopPlate",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"  17 -27 25 -26 ");
  CellMap::makeCell("TopFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "  5 -6 7 -17 (507:1000) (607:-1000)");
  CellMap::makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"  -17 -5 -218 208 ");
  CellMap::makeCell("BasePlate",System,cellIndex++,wallMat,0.0,Out);

  // Front/Back port

  // PORTS:
  // front port
  Out=ModelSupport::getComposite(SMap,buildIndex," -1000 7 -507 ");
  CellMap::makeCell("PortAVoid",System,cellIndex++,voidMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1000 17 507 -517 ");
  CellMap::makeCell("PortAWall",System,cellIndex++,wallMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -511 517 -527 ");
  CellMap::makeCell("PortAFlange",System,cellIndex++,wallMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1000 17 511 517 -527 ");
  CellMap::makeCell("PortAScreen",System,cellIndex++,voidMat,0.0,Out);

  // back port
  Out=ModelSupport::getComposite(SMap,buildIndex," 1000 7 -607 ");
  CellMap::makeCell("PortBVoid",System,cellIndex++,voidMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1000 17 607 -617 ");
  CellMap::makeCell("PortBWall",System,cellIndex++,wallMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 611 617 -627 ");
  CellMap::makeCell("PortBFlange",System,cellIndex++,wallMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1000 17 -611 617 -627 ");
  CellMap::makeCell("PortBScreen",System,cellIndex++,voidMat,0.0,Out);

  // OUTER VOID SPACE
  const std::string fbCut=FPortStr+BPortStr;
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "527 1013 -1014 5 -25 17 -1000 ");
  CellMap::makeCell("OuterFrontVoid",System,cellIndex++,0,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 627 1013 -1014 5 -25 17 1000");
  CellMap::makeCell("OuterBackVoid",System,cellIndex++,0,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1003 -1013 5 -25 17 ");
  CellMap::makeCell("OuterLeftVoid",System,cellIndex++,0,0.0,Out+fbCut);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1004 1014 5 -25 17 ");
  CellMap::makeCell("OuterRightVoid",System,cellIndex++,0,0.0,Out+fbCut);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1003 -1004 1005 -1006 25 -26 27 ");
  CellMap::makeCell("OuterFlangeVoid",System,cellIndex++,0,0.0,Out+fbCut);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1003 -1004 26 (118:17) -1006 ");
  CellMap::makeCell("OuterTopVoid",System,cellIndex++,0,0.0,Out+fbCut);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1003 -1004 -5 (218:17) 1005 ");
  CellMap::makeCell("OuterBaseVoid",System,cellIndex++,0,0.0,Out+fbCut);



  // Main exclusion box
  Out=ModelSupport::getComposite(SMap,buildIndex," 1003 -1004 1005 -1006 ");

  addOuterSurf(Out+fbCut);

  return;
}

void
TankMonoVessel::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("TankMonoVessel","createLinks");

  const Geometry::Vec3D ACentre(Origin+X*portAXStep+Z*portAZStep);
  const Geometry::Vec3D BCentre(Origin+X*portBXStep+Z*portBZStep);

  ExternalCut::createLink("front",*this,0,ACentre,-Y);
  ExternalCut::createLink("back",*this,1,BCentre,Y);

  const double xPlus=
    (voidRadius*voidRadius-topGap*topGap)/(2.0*topGap);
  const double topRadius=
    (voidRadius*voidRadius+topGap*topGap)/(2.0*topGap);
  const Geometry::Vec3D topCent=Origin-Z*(xPlus-voidHeight-topRadius);

  FixedComp::setConnect(2,topCent,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+108));


  return;
}

void
TankMonoVessel::createPorts(Simulation& System)
  /*!
    Simple function to create ports
    \param System :: Simulation to use
   */
{
   ELog::RegMethod RegA("TankMonoVessel","createPorts");

  for(size_t i=0;i<Ports.size();i++)
    {

      for(const int CN : portCells)
	Ports[i].addOuterCell(CN);

      Ports[i].setCentLine(*this,PCentre[i],PAxis[i]);
      Ports[i].constructTrack(System);
    }
  return;
}


void
TankMonoVessel::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("TankMonoVessel","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);

  createLinks();
  insertObjects(System);

  if (!delayPortBuild)
    createPorts(System);

  return;
}

}  // NAMESPACE xraySystem
