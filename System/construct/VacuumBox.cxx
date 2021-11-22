/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/VacuumBox.cxx
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
#include "geomSupport.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "VacuumBox.h"

namespace constructSystem
{

VacuumBox::VacuumBox(const std::string& Key,
		       const bool flag) : 
  attachSystem::FixedOffset(Key,10),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  centreOrigin(flag)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
    \param flag :: true if the box is centred on coordinates
  */
{}

VacuumBox::VacuumBox(const VacuumBox& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::FrontBackCut(A),
  centreOrigin(A.centreOrigin),voidHeight(A.voidHeight),
  voidWidth(A.voidWidth),voidDepth(A.voidDepth),
  voidLength(A.voidLength),feHeight(A.feHeight),
  feDepth(A.feDepth),feWidth(A.feWidth),feFront(A.feFront),
  feBack(A.feBack),portAXStep(A.portAXStep),portAZStep(A.portAZStep),
  portAWallThick(A.portAWallThick),portATubeLength(A.portATubeLength),
  portATubeRadius(A.portATubeRadius),portBXStep(A.portBXStep),
  portBZStep(A.portBZStep),portBXAngle(A.portBXAngle),
  portBZAngle(A.portBZAngle),portBWallThick(A.portBWallThick),
  portBTubeLength(A.portBTubeLength),portBTubeRadius(A.portBTubeRadius),
  flangeARadius(A.flangeARadius),flangeALength(A.flangeALength),
  flangeBRadius(A.flangeBRadius),flangeBLength(A.flangeBLength),
  voidMat(A.voidMat),feMat(A.feMat)
  /*!
    Copy constructor
    \param A :: VacuumBox to copy
  */
{}

VacuumBox&
VacuumBox::operator=(const VacuumBox& A)
  /*!
    Assignment operator
    \param A :: VacuumBox to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      voidHeight=A.voidHeight;
      voidWidth=A.voidWidth;
      voidDepth=A.voidDepth;
      voidLength=A.voidLength;
      feHeight=A.feHeight;
      feDepth=A.feDepth;
      feWidth=A.feWidth;
      feFront=A.feFront;
      feBack=A.feBack;
      portAXStep=A.portAXStep;
      portAZStep=A.portAZStep;
      portAWallThick=A.portAWallThick;
      portATubeLength=A.portATubeLength;
      portATubeRadius=A.portATubeRadius;
      portBXStep=A.portBXStep;
      portBZStep=A.portBZStep;
      portBXAngle=A.portBXAngle;
      portBZAngle=A.portBZAngle;
      portBWallThick=A.portBWallThick;
      portBTubeLength=A.portBTubeLength;
      portBTubeRadius=A.portBTubeRadius;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      voidMat=A.voidMat;
      feMat=A.feMat;
    }
  return *this;
}

  
VacuumBox::~VacuumBox() 
  /*!
    Destructor
  */
{}

void
VacuumBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VacuumBox","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");

  voidLength=Control.EvalVar<double>(keyName+"VoidLength");

  const double wallThick=Control.EvalDefVar<double>(keyName+"WallThick",0.0);
  feHeight=Control.EvalDefVar<double>(keyName+"FeHeight",wallThick);
  feDepth=Control.EvalDefVar<double>(keyName+"FeDepth",wallThick);
  feWidth=Control.EvalDefVar<double>(keyName+"FeWidth",wallThick);
  feFront=Control.EvalDefVar<double>(keyName+"FeFront",wallThick);
  feBack=Control.EvalDefVar<double>(keyName+"FeBack",wallThick);

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

  portBXAngle=Control.EvalDefVar<double>(keyName+"PortBXAngle",0.0);
  portBZAngle=Control.EvalDefVar<double>(keyName+"PortBZAngle",0.0);

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

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

  return;
}

void
VacuumBox::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("VacuumBox","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  if (!centreOrigin)
    Origin+=Y*(portATubeLength+feFront+voidLength/2.0)-
      X*portAXStep-
      Z*portAZStep;


  
  return;
}


void
VacuumBox::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VacuumBox","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
	      Origin-Y*(portATubeLength+feFront+voidLength/2.0),Y);
      setFront(SMap.realSurf(buildIndex+101));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+102,
	    Origin+Y*(portBTubeLength+feBack+voidLength/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+102));
    }
  
  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);  

  // Fe system [front face is link surf]
  ModelSupport::buildPlane(SMap,buildIndex+11,
   			   Origin-Y*(feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(voidDepth+feDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(voidHeight+feHeight),Z);

  // FRONT PORT
  const Geometry::Vec3D ACentre(Origin+X*portAXStep+Z*portAZStep);

  ModelSupport::buildCylinder(SMap,buildIndex+107,ACentre,Y,portATubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,ACentre,Y,
			      portATubeRadius+portAWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,ACentre,Y,flangeARadius);


  // BACK PORT
  const Geometry::Vec3D BCentre(Origin+X*portBXStep+Z*portBZStep);
  const Geometry::Vec3D BAxis=
    Geometry::calcRotatedVec(portBXAngle,0.0,portBZAngle,X,Y,Z,Y);
  
  ModelSupport::buildCylinder(SMap,buildIndex+207,BCentre,BAxis,portBTubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,BCentre,BAxis,
			      portBTubeRadius+portBWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,BCentre,BAxis,flangeBRadius);

  // Flange cut
  FrontBackCut::getShiftedFront(SMap,buildIndex+111,Y,flangeALength);
  FrontBackCut::getShiftedBack(SMap,buildIndex+211,BAxis,-flangeBLength);

  return;
}

void
VacuumBox::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("VacuumBox","createObjects");

  std::string Out;

  const std::string FPortStr(frontRule());
  const std::string BPortStr(backRule());
  
  // Main Void 
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // PortVoids
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -107 ");
  CellMap::makeCell("FPortVoid",System,cellIndex++,voidMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -207 ");
  CellMap::makeCell("BPortVoid",System,cellIndex++,voidMat,0.0,Out+BPortStr);

  // Main metal
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6) (1:107) (-2:207) ");
  CellMap::makeCell("MainWall",System,cellIndex++,feMat,0.0,Out);

  // Port metal
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 107 -117 ");
  CellMap::makeCell("PortWall",System,cellIndex++,feMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 207 -217 ");
  CellMap::makeCell("PortWall",System,cellIndex++,feMat,0.0,Out+BPortStr);

  // Flange
  Out=ModelSupport::getComposite(SMap,buildIndex," -111 117 -127 ");
  CellMap::makeCell("Flange",System,cellIndex++,feMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 211 217 -227 ");
  CellMap::makeCell("Flange",System,cellIndex++,feMat,0.0,Out+BPortStr);

  // Flange Voids
  Out=ModelSupport::getComposite(SMap,buildIndex," 111 -11 117 -127 ");
  CellMap::makeCell("FFlangeVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -211 217 -227 ");
  CellMap::makeCell("BFlangeVoid",System,cellIndex++,0,0.0,Out);
  

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -127 ");
  addOuterUnionSurf(Out+FPortStr);      

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -227 ");
  addOuterUnionSurf(Out+BPortStr);      
  return;
}

void
VacuumBox::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("VacuumBox","createLinks");

  const Geometry::Vec3D ACentre(Origin+X*portAXStep+Z*portAZStep);
  const Geometry::Vec3D BCentre(Origin+X*portBXStep+Z*portBZStep);
  const Geometry::Vec3D BAxis=
      Geometry::calcRotatedVec(portBXAngle,0.0,portBZAngle,X,Y,Z,Y);

  FrontBackCut::createFrontLinks(*this,ACentre,Y); 
  FrontBackCut::createBackLinks(*this,BCentre,BAxis);  

  FixedComp::setConnect(2,Origin-X*((feWidth+voidWidth)/2.0),-X);
  FixedComp::setConnect(3,Origin+X*((feWidth+voidWidth)/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(feDepth+voidDepth),-Z);
  FixedComp::setConnect(5,Origin+Z*(feHeight+voidHeight),Z);  

  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));


  FixedComp::setConnect(7,ACentre+Z*(portATubeRadius+portAWallThick),Z);
  FixedComp::setConnect(8,ACentre+Z*(portBTubeRadius+portBWallThick),Z);

  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+117));
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+217));
  
  FixedComp::nameSideIndex(2,"left");
  FixedComp::nameSideIndex(3,"right");
  FixedComp::nameSideIndex(4,"base");
  FixedComp::nameSideIndex(5,"top");
  FixedComp::nameSideIndex(7,"frontPortWall");
  FixedComp::nameSideIndex(8,"backPortWall");


  return;
}

void
VacuumBox::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VacuumBox","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE constructSystem
