/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/MonoBox.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "stringCombine.h"
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
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedSpace.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "FrontBackCut.h"
#include "portItem.h"

#include "MonoBox.h"

namespace xraySystem
{

MonoBox::MonoBox(const std::string& Key,
		       const bool flag) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedSpace(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  centreOrigin(flag)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
    \param flag :: true if the box is centred on coordinates
  */
{}


  
MonoBox::~MonoBox() 
  /*!
    Destructor
  */
{}

void
MonoBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("MonoBox","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");

  overHang=Control.EvalVar<double>(keyName+"OverHang");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
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

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

  return;
}

void
MonoBox::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("MonoBox","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  // after rotation
  if (!centreOrigin)
    {
      Origin+=
	Y*(portATubeLength+wallThick+voidLength/2.0)-
	X*portAXStep-
	Z*portAZStep;
    }
  return;
}


void
MonoBox::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("MonoBox","createSurfaces");

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

  // Walls
  ModelSupport::buildPlane(SMap,buildIndex+11,
   			   Origin-Y*(wallThick+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(wallThick+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(wallThick+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(wallThick+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(baseThick+voidDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(roofThick+voidHeight),Z);
  ModelSupport::buildPlane(SMap,buildIndex+25,
			   Origin-Z*(2.0*baseThick+voidDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(2.0*roofThick+voidHeight),Z);

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
  FrontBackCut::getShiftedFront(SMap,buildIndex+111,1,Y,flangeALength);
  FrontBackCut::getShiftedBack(SMap,buildIndex+211,-1,Y,flangeBLength);


  // Top plate:
  ModelSupport::buildPlane(SMap,buildIndex+1001,
   			   Origin-Y*(overHang+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1002,
			   Origin+Y*(overHang+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1003,
			   Origin-X*(overHang+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,
			   Origin+X*(overHang+voidWidth/2.0),X);
    
  return;
}

void
MonoBox::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("MonoBox","createObjects");

  std::string Out;

  const std::string FPortStr(frontRule());
  const std::string BPortStr(backRule());
  
  // Main Void 
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 15 -16");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // PortVoids
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -107 ");
  CellMap::makeCell("PortVoid",System,cellIndex++,voidMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -207 ");
  CellMap::makeCell("PortVoid",System,cellIndex++,voidMat,0.0,Out+BPortStr);

  // Main metal
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -12 13 -14 5 -6 (-1:2:-3:4) (-2:207) (1:107) ");
  CellMap::makeCell("MainWall",System,cellIndex++,feMat,0.0,Out);
  
  // Port metal
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 107 -117 ");
  CellMap::makeCell("PortWallA",System,cellIndex++,feMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 207 -217 ");
  CellMap::makeCell("PortWallB",System,cellIndex++,feMat,0.0,Out+BPortStr);

  // Flange
  Out=ModelSupport::getComposite(SMap,buildIndex," -111 117 -127 ");
  CellMap::makeCell("FlangeA",System,cellIndex++,feMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 211 217 -227 ");
  CellMap::makeCell("FlangeB",System,cellIndex++,feMat,0.0,Out+BPortStr);

  // Flange Voids
  Out=ModelSupport::getComposite(SMap,buildIndex," 111 -11 117 -127 ");
  CellMap::makeCell("FlangeVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -211 217 -227 ");
  CellMap::makeCell("FlangeVoid",System,cellIndex++,0,0.0,Out);
  
  //Roof:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -1002 1003 -1004 (-1:2:-3:4) 6 -16");
  CellMap::makeCell("LowRoof",System,cellIndex++,feMat,0.0,Out);
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -1002 1003 -1004 16 -26");
  CellMap::makeCell("TopRoof",System,cellIndex++,feMat,0.0,Out);

  //Base:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -1002 1003 -1004 (-1:2:-3:4) -5 15");
  CellMap::makeCell("LowBase",System,cellIndex++,feMat,0.0,Out);
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -1002 1003 -1004 -15 25");
  CellMap::makeCell("TopBase",System,cellIndex++,feMat,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -1002 1003 -1004 6 -26 ");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -1002 1003 -1004 -5 25 ");
  addOuterUnionSurf(Out);
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -12 13 -14 5 -6 ");
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -227 ");
  addOuterUnionSurf(Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -127 ");
  addOuterUnionSurf(Out);

  return;
}

void
MonoBox::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("MonoBox","createLinks");

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
MonoBox::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("MonoBox","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
