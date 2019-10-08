/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/GateValveCylinder.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell / Konstantin Batkov
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
#include "Quaternion.h"
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
#include "SpaceCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"

#include "GateValveCylinder.h" 

namespace constructSystem
{

GateValveCylinder::GateValveCylinder(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  closed(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

GateValveCylinder::GateValveCylinder(const GateValveCylinder& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),radius(A.radius),wallThick(A.wallThick),
  portARadius(A.portARadius),portAThick(A.portAThick),
  portALen(A.portALen),portBRadius(A.portBRadius),
  portBThick(A.portBThick),portBLen(A.portBLen),
  closed(A.closed),bladeLift(A.bladeLift),bladeThick(A.bladeThick),
  bladeRadius(A.bladeRadius),voidMat(A.voidMat),
  bladeMat(A.bladeMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: GateValveCylinder to copy
  */
{}

GateValveCylinder&
GateValveCylinder::operator=(const GateValveCylinder& A)
  /*!
    Assignment operator
    \param A :: GateValveCylinder to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      radius=A.radius;
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


GateValveCylinder::~GateValveCylinder() 
  /*!
    Destructor
  */
{}

void
GateValveCylinder::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("GateValveCylinder","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  liftWidth=Control.EvalVar<double>(keyName+"LiftWidth");
  liftHeight=Control.EvalVar<double>(keyName+"LiftHeight");

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


  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
GateValveCylinder::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    We set the origin external to the front face of the sealing ring.
    and adjust the origin to the middle.
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("GateValveCylinder","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  // moved to centre
  Origin+=Y*(length/2.0+portALen);
  
  return;
}


void
GateValveCylinder::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("GateValveCylinder","createSurfaces");

  // front planes
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+11,Origin-Y*(wallThick+length/2.0),Y);
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
			       Origin+Y*(portBLen+length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+102));
    }

  // sides
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  
  // Lift region:
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*liftWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*liftWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z); // mid plane
  ModelSupport::buildPlane
    (SMap,buildIndex+6,Origin+Z*(radius+liftHeight),Z);
  
  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(liftWidth+wallThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+14,Origin+X*(liftWidth+wallThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(radius+liftHeight+wallThick),Z);

  // flange

  ModelSupport::buildCylinder
    (SMap,buildIndex+107,Origin,Y,portARadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,Origin,Y,portARadius+portAThick);

  ModelSupport::buildCylinder
    (SMap,buildIndex+207,Origin,Y,portBRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,portBRadius+portBThick);

  
  // Blade
  ModelSupport::buildPlane(SMap,buildIndex+301,Origin-Y*(bladeThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(bladeThick/2.0),Y);

  const double dz(closed ? 0.0 : bladeLift);
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin+Z*dz,Y,bladeRadius);

  return;
}

void
GateValveCylinder::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GateValveCylinder","createObjects");

  std::string Out;

  const bool portAExtends(wallThick<=portALen);  // port extends
  const bool portBExtends(wallThick<=portBLen);  // port extends

  const std::string frontStr=frontRule();  // 101
  const std::string backStr=backRule();    // -102
  const std::string frontComp=frontComplement();  // -101
  const std::string backComp=backComplement();    // 102
  // Void 
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 (-7: (3 -4 5 -6))  (307:-301:302) ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // Main body
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 7 -17 (-3:4:-5) ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out);
  // Top body
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -12 17 13 -14 -16 5 (-1:2:-3:4:6) ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out);

  // blade
  Out=ModelSupport::getComposite(SMap,buildIndex," -307 301 -302 ");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,Out);

  // front plate
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 11 -17 117 ");
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
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -17 217 ");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out);
  // seal ring
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 107 -217 ");
  makeCell("BackSeal",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -207 ");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,Out+backStr);
  
  if (!portBExtends)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -12 -217 ");
      makeCell("BackVoidExtra",System,cellIndex++,voidMat,0.0,Out+backComp);
    }

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -12 (-17 : (13 -14 5 -16))");
  addOuterSurf(Out);
  if (portAExtends || portBExtends)
    {
      Out="";
      if (!portAExtends)
	Out=ModelSupport::getComposite(SMap,buildIndex," 12 -217 ");
      else if (!portBExtends)
	Out=ModelSupport::getComposite(SMap,buildIndex," -11 -117 ");
      else 
	Out=ModelSupport::getComposite(SMap,buildIndex," (-11 -117): (12 -217) ");
      
      addOuterUnionSurf(Out+frontStr+backStr);
    }

  addOuterUnionSurf(Out+frontStr+backStr);

  return;
}
  
void
GateValveCylinder::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("GateValveCylinder","createLinks");

  //stufff for intersection


  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  return;
}
  
  
void
GateValveCylinder::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("GateValveCylinder","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
