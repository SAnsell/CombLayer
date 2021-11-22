/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PortTube.cxx
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
#include <utility>
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
#include "ModelSupport.h"
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
#include "FrontBackCut.h"

#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"

namespace constructSystem
{

PortTube::PortTube(const std::string& Key) :
  PipeTube(Key),portAOuterFlag(0),portBOuterFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(2,"mainPipe");
  FixedComp::nameSideIndex(6,"portAPipe");
  FixedComp::nameSideIndex(8,"portBPipe");
  FixedComp::nameSideIndex(10,"portA");
  FixedComp::nameSideIndex(11,"portB");
}

  
PortTube::~PortTube() 
  /*!
    Destructor
  */
{}

void
PortTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("PortTube","populate");

  PipeTube::populate(Control);

  portAOuterFlag=
    (Control.EvalDefVar<int>(keyName+"PortAOuterFlag",0)!=0);
  portAXStep=Control.EvalDefVar<double>(keyName+"PortAXStep",0.0);
  portAZStep=Control.EvalDefVar<double>(keyName+"PortAZStep",0.0);
  portARadius=Control.EvalPair<double>(keyName+"PortARadius",
				       keyName+"PortRadius");
  portALen=Control.EvalPair<double>(keyName+"PortALen",
				       keyName+"PortLen");
  portAThick=Control.EvalPair<double>(keyName+"PortAThick",
				       keyName+"PortThick");
  portBOuterFlag=
    (Control.EvalDefVar<int>(keyName+"PortBOuterFlag",0)!=0);

  portBXStep=Control.EvalDefVar<double>(keyName+"PortBXStep",0.0);
  portBZStep=Control.EvalDefVar<double>(keyName+"PortBZStep",0.0);
  portBRadius=Control.EvalPair<double>(keyName+"PortBRadius",
				       keyName+"PortRadius");
  portBLen=Control.EvalPair<double>(keyName+"PortBLen",
				       keyName+"PortLen");
  portBThick=Control.EvalPair<double>(keyName+"PortBThick",
				       keyName+"PortThick");
				    
  return;
}



void
PortTube::applyPortRotation()
  /*!
    Apply a rotation to all the PCentre and the 
    PAxis of the ports
  */
{
  ELog::RegMethod RegA("PortTube","applyPortRotation");

  PipeTube::applyPortRotation();
  if (portConnectIndex==1 || portConnectIndex==2)
    Origin+=Y*(portALen+wallThick)-X*portAXStep-Z*portAZStep;
  return;
}

void
PortTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("PortTube","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
			       Origin-Y*(portALen+wallThick+length/2.0),Y);
      setFront(SMap.realSurf(buildIndex+101));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+202,
			       Origin+Y*(portBLen+wallThick+length/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+202));
    }

  // void space:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  SurfMap::addSurf("VoidFront",SMap.realSurf(buildIndex+1));
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  SurfMap::addSurf("VoidBack",-SMap.realSurf(buildIndex+2));
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  SurfMap::addSurf("VoidCyl",-SMap.realSurf(buildIndex+7));
  
  // metal
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(wallThick+length/2.0),Y);
  SurfMap::addSurf("PortACut",SMap.realSurf(buildIndex+11));
  
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(wallThick+length/2.0),Y);
  SurfMap::addSurf("PortBCut",SMap.realSurf(buildIndex+12));

  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);
  SurfMap::addSurf("OuterCyl",SMap.realSurf(buildIndex+17));
  // port
  const Geometry::Vec3D inOrg=Origin+X*portAXStep+Z*portAZStep;
  const Geometry::Vec3D outOrg=Origin+X*portBXStep+Z*portBZStep;

    
  ModelSupport::buildCylinder(SMap,buildIndex+107,inOrg,Y,portARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,outOrg,Y,portBRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+117,inOrg,Y,
			      portARadius+portAThick);
  ModelSupport::buildCylinder(SMap,buildIndex+217,outOrg,Y,
			      portBRadius+portBThick);

  ModelSupport::buildPlane(SMap,buildIndex+111,Origin-
			   Y*(portALen+wallThick-flangeALength+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+212,Origin+
			   Y*(portBLen+wallThick-flangeBLength+length/2.0),Y);

  // flange:
    
  ModelSupport::buildCylinder(SMap,buildIndex+127,inOrg,Y,
			      flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+227,outOrg,Y,
			      flangeBRadius);
  
  return;
}

void
PortTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("PortTube","createObjects");

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  // main walls
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -17 7 -2 ");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,Out);

  // plates front/back
  if ((portARadius+portAThick-(radius+wallThick))< -Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -1 11 -17 117 ");
      makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,Out);
    }
  if ((portBRadius+portBThick-radius-wallThick)< -Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -17 217 ");
      makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out);
    }

  // port:
  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -1  -107 ");
  makeCell("FrontPortVoid",System,cellIndex++,voidMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -207 ");
  makeCell("BackPortVoid",System,cellIndex++,voidMat,0.0,Out+backSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 107 -117");
  makeCell("FrontPortWall",System,cellIndex++,wallMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 207 -217 ");
  makeCell("BackPortWall",System,cellIndex++,wallMat,0.0,Out+backSurf);

  // flanges
  Out=ModelSupport::getComposite(SMap,buildIndex," -111 117 -127 ");
  makeCell("FrontPortFlange",System,cellIndex++,wallMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 212 217 -227 ");
  makeCell("BackPortFlange",System,cellIndex++,wallMat,0.0,Out+backSurf);

  // OUTER SURF:
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 -17 ");
  addOuterSurf("Main",Out);
  
  // outer void
  if (portAOuterFlag)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -11 111 117 -127 ");
      makeCell("FrontPortOuterVoid",System,cellIndex++,0,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,"-11 -127 ");
      addOuterSurf("FlangeA",Out+frontSurf);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"-11 -127 (-117:-111) ");
      addOuterSurf("FlangeA",Out+frontSurf);
    }

  if (portBOuterFlag)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 12 -212 117 -227 ");
      makeCell("BackPortOuterVoid",System,cellIndex++,0,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 12 -227 ");
      addOuterSurf("FlangeB",Out+backSurf);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 12 -227 ( -217:212 ) ");
      addOuterSurf("FlangeB",Out+backSurf);
    }     
  return;
}

void
PortTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("PortTube","createLinks");

  // port centre
  const Geometry::Vec3D inOrg=Origin+X*portAXStep+Z*portAZStep;
  const Geometry::Vec3D outOrg=Origin+X*portBXStep+Z*portBZStep;
  
  FrontBackCut::createFrontLinks(*this,inOrg,Y); 
  FrontBackCut::createBackLinks(*this,outOrg,Y);  

  FixedComp::setConnect(2,Origin-X*(radius+wallThick),-X);
  FixedComp::setConnect(3,Origin+X*(radius+wallThick),X);
  FixedComp::setConnect(4,Origin-Z*(radius+wallThick),-Z);
  FixedComp::setConnect(5,Origin+Z*(radius+wallThick),Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+17));

  const Geometry::Vec3D AVec(Origin+X*portAXStep+Z*portAZStep);
  const Geometry::Vec3D BVec(Origin+X*portBXStep+Z*portBZStep);
  FixedComp::setConnect(6,AVec-Z*(portARadius+portAThick),-Z);
  FixedComp::setConnect(7,AVec+Z*(portARadius+portAThick),Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+117));
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+117));

  FixedComp::setConnect(8,BVec-Z*(portBRadius+portBThick),-Z);
  FixedComp::setConnect(9,BVec+Z*(portBRadius+portBThick),Z);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+217));
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+217));

  FixedComp::setConnect(10,AVec-Y*(wallThick+length/2.0),-Y);
  FixedComp::setConnect(11,BVec+Y*(wallThick+length/2.0),Y);
  FixedComp::setLinkSurf(10,-SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(11,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(12,Origin,Y);

  if (!outerVoid)
    {
      FixedComp::setConnect(13,Origin+Z*(radius+wallThick),Z);
      FixedComp::setLinkSurf(13,SMap.realSurf(buildIndex+17));
    }
  else
    {
      FixedComp::setConnect(13,Origin+Z*flangeBRadius,Z);
      FixedComp::setLinkSurf(13,SMap.realSurf(buildIndex+107));
    }
  nameSideIndex(7,"OuterRadius");

  
  return;
}

}  // NAMESPACE constructSystem
