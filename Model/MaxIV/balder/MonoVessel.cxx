/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/MonoVessel.cxx
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
#include "SurfMap.h"
#include "portItem.h"

#include "MonoVessel.h"

namespace xraySystem
{

MonoVessel::MonoVessel(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

MonoVessel::MonoVessel(const MonoVessel& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  radius(A.radius),ringWidth(A.ringWidth),outWidth(A.outWidth),
  wallThick(A.wallThick),doorThick(A.doorThick),
  backThick(A.backThick),doorFlangeLen(A.doorFlangeLen),
  doorFlangeRad(A.doorFlangeRad),ringFlangeLen(A.ringFlangeLen),
  ringFlangeRad(A.ringFlangeRad),inPortZStep(A.inPortZStep),
  inPortRadius(A.inPortRadius),inPortLen(A.inPortLen),
  inPortThick(A.inPortThick),inPortFlangeRad(A.inPortFlangeRad),
  inPortFlangeLen(A.inPortFlangeLen),outPortZStep(A.outPortZStep),
  outPortRadius(A.outPortRadius),outPortLen(A.outPortLen),
  outPortThick(A.outPortThick),outPortFlangeRad(A.outPortFlangeRad),
  outPortFlangeLen(A.outPortFlangeLen),voidMat(A.voidMat),
  wallMat(A.wallMat),inPortPt(A.inPortPt),outPortPt(A.outPortPt)
  /*!
    Copy constructor
    \param A :: MonoVessel to copy
  */
{}

MonoVessel&
MonoVessel::operator=(const MonoVessel& A)
  /*!
    Assignment operator
    \param A :: MonoVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      radius=A.radius;
      ringWidth=A.ringWidth;
      outWidth=A.outWidth;
      wallThick=A.wallThick;
      doorThick=A.doorThick;
      backThick=A.backThick;
      doorFlangeLen=A.doorFlangeLen;
      doorFlangeRad=A.doorFlangeRad;
      ringFlangeLen=A.ringFlangeLen;
      ringFlangeRad=A.ringFlangeRad;
      inPortZStep=A.inPortZStep;
      inPortRadius=A.inPortRadius;
      inPortLen=A.inPortLen;
      inPortThick=A.inPortThick;
      inPortFlangeRad=A.inPortFlangeRad;
      inPortFlangeLen=A.inPortFlangeLen;
      outPortZStep=A.outPortZStep;
      outPortRadius=A.outPortRadius;
      outPortLen=A.outPortLen;
      outPortThick=A.outPortThick;
      outPortFlangeRad=A.outPortFlangeRad;
      outPortFlangeLen=A.outPortFlangeLen;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      inPortPt=A.inPortPt;
      outPortPt=A.outPortPt;
    }
  return *this;
}


MonoVessel::~MonoVessel() 
  /*!
    Destructor
  */
{}

void
MonoVessel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("MonoVessel","populate");

  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  ringWidth=Control.EvalVar<double>(keyName+"RingWidth");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  doorThick=Control.EvalVar<double>(keyName+"DoorThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");

  ringFlangeLen=Control.EvalPair<double>(keyName+"RingFlangeLen",
					 keyName+"FlangeLen");
  ringFlangeRad=Control.EvalPair<double>(keyName+"RingFlangeRad",
					 keyName+"FlangeRad");
  doorFlangeLen=Control.EvalPair<double>(keyName+"DoorFlangeLen",
					 keyName+"FlangeLen");
  doorFlangeRad=Control.EvalPair<double>(keyName+"DoorFlangeRad",
					 keyName+"FlangeRad");


  // IN PORT
  inPortZStep=Control.EvalVar<double>(keyName+"InPortZStep");

  inPortRadius=Control.EvalPair<double>(keyName+"InPortRadius",
					keyName+"PortRadius");
  inPortLen=Control.EvalPair<double>(keyName+"InPortLen",keyName+"PortLen");
  inPortThick=Control.EvalPair<double>(keyName+"InPortThick",
					  keyName+"PortThick");
  inPortFlangeRad=Control.EvalPair<double>(keyName+"InPortFlangeRad",
					   keyName+"PortFlangeRad");
  inPortFlangeLen=Control.EvalPair<double>(keyName+"InPortFlangeLen",
					   keyName+"PortFlangeLen");

  // Out PORT
  outPortZStep=Control.EvalVar<double>(keyName+"OutPortZStep");

  outPortRadius=Control.EvalPair<double>(keyName+"OutPortRadius",
					keyName+"PortRadius");
  outPortLen=Control.EvalPair<double>(keyName+"OutPortLen",
				     keyName+"PortLen");
  outPortThick=Control.EvalPair<double>(keyName+"OutPortThick",
					  keyName+"PortThick");
  outPortFlangeRad=Control.EvalPair<double>(keyName+"OutPortFlangeRad",
					   keyName+"PortFlangeRad");
  outPortFlangeLen=Control.EvalPair<double>(keyName+"OutPortFlangeLen",
					   keyName+"PortFlangeLen");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

 
void
MonoVessel::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("MonoVessel","createSurfaces");

  // Inner void
  // central dividing plane
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*ringWidth,X);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,X,radius);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(outWidth+doorThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(ringWidth+backThick),X);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,X,radius+wallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,X,radius+doorFlangeRad);
  ModelSupport::buildCylinder(SMap,buildIndex+28,Origin,X,radius+ringFlangeRad);
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(outWidth-doorFlangeLen),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(ringWidth-ringFlangeLen),X);

  // Inner port:
  inPortPt=Origin+Z*inPortZStep-
    Y*sqrt(radius*radius-inPortZStep*inPortZStep);
  
  ModelSupport::buildCylinder(SMap,buildIndex+107,inPortPt,Y,inPortRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,inPortPt,Y,
			      inPortRadius+inPortThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,inPortPt,Y,
			      inPortRadius+inPortFlangeRad);
  ModelSupport::buildPlane(SMap,buildIndex+101,inPortPt-Y*inPortLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+111,
			   inPortPt-Y*(inPortLen-inPortFlangeLen),Y);

  // Out port:
  outPortPt=Origin+Z*outPortZStep+
    Y*sqrt(radius*radius-outPortZStep*outPortZStep);
  
  ModelSupport::buildCylinder(SMap,buildIndex+207,outPortPt,Y,outPortRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,outPortPt,Y,
			      outPortRadius+outPortThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,outPortPt,Y,
			      outPortRadius+outPortFlangeRad);
  ModelSupport::buildPlane(SMap,buildIndex+201,outPortPt+Y*outPortLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+211,
			   outPortPt+Y*(outPortLen-outPortFlangeLen),Y);

  return;
}

void
MonoVessel::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MonoVessel","createObjects");

  std::string Out;

  // main void
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 3 -4 ");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // in-port void
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 101 -1 -107");
  CellMap::makeCell("InPortVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 7 101 107 -117");
  CellMap::makeCell("InPortWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -111 117 -127");
  CellMap::makeCell("InPortFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 17 111 117 -127");
  CellMap::makeCell("InPortFVoid",System,cellIndex++,0,0.0,Out);

  // out-port: void
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 7 -201 -207");
  CellMap::makeCell("OutPortVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 7 -201 207 -217");
  CellMap::makeCell("OutPortWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -201 211 217 -227");
  CellMap::makeCell("OutPortFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 17 -211 217 -227");
  CellMap::makeCell("OutPortFVoid",System,cellIndex++,0,0.0,Out);

  // MAIN WALL:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 7 -17 3 -4 (-1:217) (1:117)");
  CellMap::makeCell("MainWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -27 13 -3 ");
  CellMap::makeCell("Door",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -27 3 -23 ");
  CellMap::makeCell("DoorFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 4 -14 -28");
  CellMap::makeCell("Back",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -28 -4 24 ");
  CellMap::makeCell("BackFlange",System,cellIndex++,wallMat,0.0,Out);

  // Exclude Main/port/port:
  Out=ModelSupport::getComposite(SMap,buildIndex," 23 -24 -17 ");
  addOuterSurf(Out);      

  // flanges:
  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -23 -27 ");
  addOuterUnionSurf(Out);      
  Out=ModelSupport::getComposite(SMap,buildIndex," 24 -14 -28 ");
  addOuterUnionSurf(Out);
  // ports:
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -201 -227");
  addOuterUnionSurf(Out);      
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 101 -127");
  addOuterUnionSurf(Out);      

  return;
}

void
MonoVessel::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("MonoVessel","createLinks");

  FixedComp::setConnect(0,inPortPt-Y*inPortLen,Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+101));
  FixedComp::setConnect(1,outPortPt+Y*outPortLen,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+201));
  return;
}

void
MonoVessel::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("MonoVessel","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System);
  
  createLinks();
  if (!ContainedComp::insertCells.empty())
    {
      insertObjects(System);   
      constructPorts(System);
    }
  
  return;
}


void
MonoVessel::constructPorts(Simulation& System)
  /*!
    Separate call to construct port because it may 
    need to be called late
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("MonoVessel","constructPorts");

  
  //  constructSystem::portItem windowPort(keyName+"Port0");
  constructSystem::portItem windowPort(keyName+"Port",keyName+"Port0");
  windowPort.createAll(System,*this,0);
  return;  
}
  
}  // NAMESPACE xraySystem
