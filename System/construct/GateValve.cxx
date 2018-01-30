/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/GateValve
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
#include "Qhull.h"
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
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"

#include "GateValve.h" 

namespace constructSystem
{

GateValve::GateValve(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1),closed(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

GateValve::GateValve(const GateValve& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),  
  vacIndex(A.vacIndex),cellIndex(A.cellIndex),length(A.length),
  width(A.width),height(A.height),depth(A.depth),
  wallThick(A.wallThick),portRadius(A.portRadius),
  portThick(A.portThick),portLen(A.portLen),closed(A.closed),
  bladeLift(A.bladeLift),bladeThick(A.bladeThick),
  bladeRadius(A.bladeRadius),voidMat(A.voidMat),
  bladeMat(A.bladeMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: GateValve to copy
  */
{}

GateValve&
GateValve::operator=(const GateValve& A)
  /*!
    Assignment operator
    \param A :: GateValve to copy
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
      cellIndex=A.cellIndex;
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      portRadius=A.portRadius;
      portThick=A.portThick;
      portLen=A.portLen;
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


GateValve::~GateValve() 
  /*!
    Destructor
  */
{}

void
GateValve::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("GateValve","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portThick=Control.EvalVar<double>(keyName+"PortThick");
  portLen=Control.EvalVar<double>(keyName+"PortLen");
  
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
GateValve::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    We set the origin external to the front face of the sealing ring.
    and adjust the origin to the middle.
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("GateValve","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  // moved to centre
  Origin+=Y*(length/2.0+portLen);
  
  return;
}


void
GateValve::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("GateValve","createSurfaces");

  // front planes
  ModelSupport::buildPlane(SMap,vacIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+11,Origin-Y*(wallThick+length/2.0),Y);
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+101,
			       Origin-Y*(portLen+length/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(vacIndex+101));
    }
  
  // back planes
  ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+12,Origin+Y*(wallThick+length/2.0),Y);
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+102,
			       Origin+Y*(portLen+length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(vacIndex+102));
    }

  // sides
  ModelSupport::buildPlane(SMap,vacIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,vacIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,vacIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,vacIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,vacIndex+13,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,vacIndex+14,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,vacIndex+15,Origin-Z*(depth+wallThick),Z);
  ModelSupport::buildPlane(SMap,vacIndex+16,Origin+Z*(height+wallThick),Z);

  // flange

  ModelSupport::buildCylinder(SMap,vacIndex+107,Origin,Y,portRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+117,Origin,Y,portRadius+portThick);

  
  // Blade
  ModelSupport::buildPlane(SMap,vacIndex+201,Origin-Y*(bladeThick/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+202,Origin+Y*(bladeThick/2.0),Y);

  if (closed)
    ModelSupport::buildCylinder(SMap,vacIndex+207,Origin,Y,bladeRadius);
  else
    ModelSupport::buildCylinder(SMap,vacIndex+207,Origin+Z*bladeLift,
				Y,bladeRadius);

  return;
}

void
GateValve::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GateValve","createObjects");

  std::string Out;

  const bool portExtends(wallThick<=portLen);  // port extends

  const std::string frontStr=frontRule();  // 101
  const std::string backStr=backRule();    // -102
  const std::string frontComp=frontComplement();  // -101
  const std::string backComp=backComplement();    // 102
  // Void 
  Out=ModelSupport::getComposite(SMap,vacIndex,
				 " 1 -2 3 -4 5 -6 (207:-201:202) ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // Main body
  Out=ModelSupport::getComposite(SMap,vacIndex,
				 " 1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out);

  // blade
  Out=ModelSupport::getComposite(SMap,vacIndex," -207 201 -202 ");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,Out);

  // front plate
  Out=ModelSupport::getComposite(SMap,vacIndex," -1 11 13 -14 15 -16 117 ");
  makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,Out);
  // seal ring
  Out=ModelSupport::getComposite(SMap,vacIndex," -1 107 -117 ");
  makeCell("FrontSeal",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,vacIndex," -1 -107 ");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);
  
  if (!portExtends)
    {
      Out=ModelSupport::getComposite(SMap,vacIndex," 11 -117 ");
      makeCell("FrontVoidExtra",System,cellIndex++,voidMat,0.0,Out+frontComp);
    }
       
  // back plate
  Out=ModelSupport::getComposite(SMap,vacIndex," 2 -12 13 -14 15 -16 117 ");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out);
  // seal ring
  Out=ModelSupport::getComposite(SMap,vacIndex," 2 107 -117 ");
  makeCell("BackSeal",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,vacIndex," 2 -107 ");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,Out+backStr);
  
  if (!portExtends)
    {
      Out=ModelSupport::getComposite(SMap,vacIndex," -12 -117 ");
      makeCell("BackVoidExtra",System,cellIndex++,voidMat,0.0,Out+backComp);
    }

  Out=ModelSupport::getComposite(SMap,vacIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf(Out);
  if (portExtends)
    {
      Out=ModelSupport::getComposite(SMap,vacIndex," -117 ");
      addOuterUnionSurf(Out+frontStr+backStr);
    }
      

  return;
}
  
void
GateValve::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("GateValve","createLinks");

  //stufff for intersection


  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  return;
}
  
  
void
GateValve::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("GateValve","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
