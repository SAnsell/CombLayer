/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/GrateMonoBox.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "portItem.h"

#include "GrateMonoBox.h"

namespace xraySystem
{

GrateMonoBox::GrateMonoBox(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),centreOrigin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
    \param flag :: true if the box is centred on coordinates
  */
{}

GrateMonoBox::~GrateMonoBox()
  /*!
    Destructor
  */
{}

void
GrateMonoBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("GrateMonoBox","populate");

  FixedOffset::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");

  overHangExtent=Control.EvalVar<double>(keyName+"OverHangExtent");
  overHangDepth=Control.EvalVar<double>(keyName+"OverHangDepth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  radiusThick=Control.EvalVar<double>(keyName+"RadiusThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

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
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");

  if (flangeARadius<(portAWallThick+portATubeRadius+Geometry::zeroTol))
    throw ColErr::SizeError<double>
      (flangeARadius,portAWallThick+portATubeRadius,
       "Flange to small for "+keyName+" port A");

  if (flangeBRadius<(portBWallThick+portBTubeRadius+Geometry::zeroTol))
    throw ColErr::SizeError<double>
      (flangeBRadius,portBWallThick+portBTubeRadius,
       "Flange to small for "+keyName+" port B");


  const size_t NPorts=Control.EvalVar<size_t>(keyName+"NPorts");
  const std::string portBase=keyName+"Port";
  //  double L,R,W,FR,FT,CT;
  //  int capMat;
  //  int OFlag;
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


  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
GrateMonoBox::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("GrateMonoBox","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  Origin+=Y*(portATubeLength+wallThick+voidLength/2.0);
  return;
}

void
GrateMonoBox::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("GrateMonoBox","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
	      Origin-Y*(portATubeLength+wallThick+voidLength/2.0),Y);
      setFront(SMap.realSurf(buildIndex+101));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+102,
	    Origin+Y*(portBTubeLength+wallThick+voidLength/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+102));
    }


  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);
  ModelSupport::buildCylinder
    (SMap,buildIndex+7,Origin-Z*voidDepth,Y,voidRadius);

  // Walls
  ModelSupport::buildPlane(SMap,buildIndex+11,
   			   Origin-Y*(wallThick+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(wallThick+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(wallThick+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(wallThick+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(roofThick+voidHeight),Z);
  ModelSupport::buildCylinder
    (SMap,buildIndex+17,Origin-Z*voidDepth,Y,voidRadius+radiusThick);

  // FRONT PORT
  const Geometry::Vec3D ACentre(Origin+X*portAXStep+Z*portAZStep);
  ModelSupport::buildCylinder(SMap,buildIndex+107,ACentre,Y,portATubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,ACentre,Y,
			      portATubeRadius+portAWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,ACentre,Y,flangeARadius);


  // BACK PORT
  const Geometry::Vec3D BCentre(Origin+X*portBXStep+Z*portBZStep);
  ModelSupport::buildCylinder(SMap,buildIndex+207,BCentre,Y,portBTubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,BCentre,Y,
			      portBTubeRadius+portBWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,BCentre,Y,flangeBRadius);

  // Flange cut
  FrontBackCut::getShiftedFront(SMap,buildIndex+111,Y,flangeALength);
  FrontBackCut::getShiftedBack(SMap,buildIndex+211,Y,-flangeBLength);


  // Top plate:
  ModelSupport::buildPlane(SMap,buildIndex+1001,
   			   Origin-Y*(overHangExtent+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1002,
			   Origin+Y*(overHangExtent+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1003,
			   Origin-X*(overHangExtent+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,
			   Origin+X*(overHangExtent+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1006,
			   Origin+Z*(voidHeight-overHangDepth),Z);
  ModelSupport::buildCylinder
    (SMap,buildIndex+1007,Origin-Z*voidDepth,Y,voidRadius+overHangExtent);

  return;
}

void
GrateMonoBox::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("GrateMonoBox","createObjects");

  std::string Out;

  const std::string FPortStr(frontRule());
  const std::string BPortStr(backRule());

  // Main Void
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  // Base Void
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7 -5");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // main tank skins
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 13 -3 5 -6");
  CellMap::makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 4 -14 5 -6");
  CellMap::makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"11 -1 13 -14 (-17 : 5) -6 107 ");
  CellMap::makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-12 2 13 -14 (-17 : 5) -6 207 ");
  CellMap::makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 7 -17 -5");
  CellMap::makeCell("RoundWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -1002 1003 -1004 6 -16");
  CellMap::makeCell("Top",System,cellIndex++,wallMat,0.0,Out);

  // Screen voids
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 1003 -13 5 -6 ");
  CellMap::makeCell("Screen",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 14 -1004  5 -6");
  CellMap::makeCell("Screen",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"1001 -11  1003 -1004 (-1007:5) -6 127 ");
  CellMap::makeCell("FrontScreen",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"12 -1002  1003 -1004 (-1007:5) -6 227 ");
  CellMap::makeCell("BackScreen",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 17 -1007 -5");
  CellMap::makeCell("RoundScreen",System,cellIndex++,voidMat,0.0,Out);


  // PortVoids
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -107 ");
  CellMap::makeCell("PortAVoid",System,cellIndex++,voidMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -11 107 -117 ");
  CellMap::makeCell("PortAWall",System,cellIndex++,wallMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -111 117 -127 ");
  CellMap::makeCell("PortAFlange",System,cellIndex++,wallMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -11 111 117 -127 ");
  CellMap::makeCell("PortAScreen",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -207 ");
  CellMap::makeCell("PortBVoid",System,cellIndex++,voidMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 207 -217 ");
  CellMap::makeCell("PortBWall",System,cellIndex++,wallMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 211 217 -227 ");
  CellMap::makeCell("PortBFlange",System,cellIndex++,wallMat,0.0,Out+BPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -211 217 -227 ");
  CellMap::makeCell("PortBScreen",System,cellIndex++,voidMat,0.0,Out);



  Out=ModelSupport::getComposite
    (SMap,buildIndex,"1001 -1002 1003 -1004 5 -16 ");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"1001 -1002 -1007 -5 ");
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -227 ");
  addOuterUnionSurf(Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -127 ");
  addOuterUnionSurf(Out);

  return;
}

void
GrateMonoBox::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("GrateMonoBox","createLinks");

  const Geometry::Vec3D ACentre(Origin+X*portAXStep+Z*portAZStep);
  const Geometry::Vec3D BCentre(Origin+X*portBXStep+Z*portBZStep);

  FrontBackCut::createFrontLinks(*this,ACentre,Y);
  FrontBackCut::createBackLinks(*this,BCentre,Y);
  // FixedComp::setConnect(2,Origin-X*((width+voidWidth)/2.0),-X);
  // FixedComp::setConnect(3,Origin+X*((feWidth+voidWidth)/2.0),X);
  // FixedComp::setConnect(4,Origin-Z*(feDepth+voidDepth),-Z);
  // FixedComp::setConnect(5,Origin+Z*(feHeight+voidHeight),Z);

  // FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  // FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));
  // FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  // FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}

void
GrateMonoBox::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("GrateMonoBox","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);

  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
