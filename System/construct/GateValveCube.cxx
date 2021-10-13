/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/GateValveCube.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "GateValveCube.h" 

namespace constructSystem
{

GateValveCube::GateValveCube(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  closed(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

GateValveCube::GateValveCube(const GateValveCube& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),wallThick(A.wallThick),portARadius(A.portARadius),
  portAThick(A.portAThick),portALen(A.portALen),
  portBRadius(A.portBRadius),portBThick(A.portBThick),
  portBLen(A.portBLen),closed(A.closed),bladeLift(A.bladeLift),
  bladeThick(A.bladeThick),bladeRadius(A.bladeRadius),
  voidMat(A.voidMat),bladeMat(A.bladeMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: GateValveCube to copy
  */
{}

GateValveCube&
GateValveCube::operator=(const GateValveCube& A)
  /*!
    Assignment operator
    \param A :: GateValveCube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      portARadius=A.portARadius;
      portAThick=A.portAThick;
      portALen=A.portALen;
      portBRadius=A.portBRadius;
      portBThick=A.portBThick;
      portBLen=A.portBLen;
      closed=A.closed;
      bladeLift=A.bladeLift;
      bladeThick=A.bladeThick;
      bladeRadius=A.bladeRadius;
      voidMat=A.voidMat;
      bladeMat=A.bladeMat;
      wallMat=A.wallMat;
    }
  return *this;
}

  
GateValveCube::~GateValveCube() 
  /*!
    Destructor
  */
{}

void
GateValveCube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("GateValveCube","populate");
  
  FixedRotate::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  portARadius=Control.EvalPair<double>(keyName+"PortARadius",
				       keyName+"PortRadius");
  portAThick=Control.EvalPair<double>(keyName+"PortAThick",
				      keyName+"PortThick");
  portALen=Control.EvalPair<double>(keyName+"PortALen",
				    keyName+"PortLen");

  portBRadius=Control.EvalPair<double>(keyName+"PortBRadius",
				       keyName+"PortRadius");
  portBThick=Control.EvalPair<double>(keyName+"PortBThick",
				      keyName+"PortThick");
  portBLen=Control.EvalPair<double>(keyName+"PortBLen",
				    keyName+"PortLen");
  
  closed=Control.EvalDefVar<int>(keyName+"Closed",closed);
  bladeLift=Control.EvalVar<double>(keyName+"BladeLift");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeRadius=Control.EvalVar<double>(keyName+"BladeRadius");


  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
GateValveCube::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    We set the origin external to the front face of the sealing ring.
    and adjust the origin to the middle.
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("GateValveCube","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  // moved to centre
  Origin+=Y*(length/2.0+portALen);
  
  return;
}


void
GateValveCube::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("GateValveCube","createSurfaces");

  // front planes
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(wallThick+length/2.0),Y);
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
			       Origin-Y*(portALen+length/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+101));
    }
  
  // back planes
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(wallThick+length/2.0),Y);
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+102,
			       Origin+Y*(portALen+length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+102));
    }

  // sides
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+wallThick),Z);

  // flange

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,portARadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,Origin,Y,portARadius+portAThick);

  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,portBRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,portBRadius+portBThick);

  
  // Blade
  ModelSupport::buildPlane(SMap,buildIndex+301,Origin-Y*(bladeThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(bladeThick/2.0),Y);

  if (closed)
    ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,bladeRadius);
  else
    ModelSupport::buildCylinder(SMap,buildIndex+307,Origin+Z*bladeLift,
				Y,bladeRadius);

  return;
}

void
GateValveCube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GateValveCube","createObjects");

  std::string Out;

  const bool portAExtends(wallThick<=portALen);  // port extends
  const bool portBExtends(wallThick<=portBLen);  // port extends

  const std::string frontStr=frontRule();  // 101
  const std::string backStr=backRule();    // -102
  const std::string frontComp=frontComplement();  // -101
  const std::string backComp=backComplement();    // 102
  // Void 
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 3 -4 5 -6 (307:-301:302) ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // Main body
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out);

  // blade
  Out=ModelSupport::getComposite(SMap,buildIndex," -307 301 -302 ");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,Out);

  // front plate
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 11 13 -14 15 -16 117 ");
  makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,Out);
  // seal ring
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 107 -117 ");
  makeCell("FrontSeal",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -107 ");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);
  
  if (!portAExtends)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 11 -117 ");
      makeCell("FrontVoidExtra",System,cellIndex++,voidMat,0.0,Out+frontComp);
    }
       
  // back plate
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 13 -14 15 -16 217 ");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out);
  // seal ring
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 207 -217 ");
  makeCell("BackSeal",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -207 ");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,Out+backStr);
  
  if (!portBExtends)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -12 -217 ");
      makeCell("BackVoidExtra",System,cellIndex++,voidMat,0.0,Out+backComp);
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf(Out);


  if (portAExtends || portBExtends)
    {
      if (!portAExtends)
	Out=ModelSupport::getComposite(SMap,buildIndex," 12 -217 ");
      else if (!portBExtends)
	Out=ModelSupport::getComposite(SMap,buildIndex," -11 -117 ");
      else 
	Out=ModelSupport::getComposite
	  (SMap,buildIndex," ((-11 -117) : (12 -217)) ");

      addOuterUnionSurf(Out+frontStr+backStr);
    }
      
  return;
}
  
void
GateValveCube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("GateValveCube","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  return;
}
  
  
void
GateValveCube::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("GateValveCube","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
