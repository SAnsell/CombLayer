/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/DiskChopper.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"

#include "Jaws.h"

namespace constructSystem
{

Jaws::Jaws(const std::string& Key) : 
  attachSystem::FixedComp(Key,6),attachSystem::ContainedComp(),
  jawIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(jawIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Jaws::Jaws(const Jaws& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  jawIndex(A.jawIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),zOpen(A.zOpen),zThick(A.zThick),
  zCross(A.zCross),zLen(A.zLen),xOpen(A.xOpen),
  xThick(A.xThick),xCross(A.xCross),xLen(A.xLen),
  jawGap(A.jawGap),XHeight(A.XHeight),YHeight(A.YHeight),
  ZHeight(A.ZHeight),wallThick(A.wallThick),
  zJawMat(A.zJawMat),xJawMat(A.xJawMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: Jaws to copy
  */
{}

Jaws&
Jaws::operator=(const Jaws& A)
  /*!
    Assignment operator
    \param A :: Jaws to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      zOpen=A.zOpen;
      zThick=A.zThick;
      zCross=A.zCross;
      zLen=A.zLen;
      xOpen=A.xOpen;
      xThick=A.xThick;
      xCross=A.xCross;
      xLen=A.xLen;
      jawGap=A.jawGap;
      XHeight=A.XHeight;
      YHeight=A.YHeight;
      ZHeight=A.ZHeight;
      wallThick=A.wallThick;
      zJawMat=A.zJawMat;
      xJawMat=A.xJawMat;
      wallMat=A.wallMat;
    }
  return *this;
}

Jaws::~Jaws() 
  /*!
    Destructor
  */
{}


void
Jaws::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Jaws","populate");
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  zOpen=Control.EvalVar<double>(keyName+"ZOpen");
  zThick=Control.EvalVar<double>(keyName+"ZThick");
  zCross=Control.EvalVar<double>(keyName+"ZCross");
  zLen=Control.EvalVar<double>(keyName+"ZLen");

  xOpen=Control.EvalVar<double>(keyName+"XOpen");
  xThick=Control.EvalVar<double>(keyName+"XThick");
  xCross=Control.EvalVar<double>(keyName+"XCross");
  xLen=Control.EvalVar<double>(keyName+"XLen");

  jawGap=Control.EvalVar<double>(keyName+"Gap");

  XHeight=Control.EvalVar<double>(keyName+"XHeight");
  YHeight=Control.EvalVar<double>(keyName+"YHeight");
  ZHeight=Control.EvalVar<double>(keyName+"ZHeight");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  zJawMat=ModelSupport::EvalMat<int>(Control,keyName+"zJawMat");
  xJawMat=ModelSupport::EvalMat<int>(Control,keyName+"xJawMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  return;
}

void
Jaws::createUnitVector(const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Jaws","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  ELog::EM<<"Z == "<<Z<<" "<<FC.getZ()<<ELog::endDiag;
  return;
}

void
Jaws::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Jaws","createSurfaces");

  // Primary outside layers:
  
  ModelSupport::buildPlane(SMap,jawIndex+1,Origin-Y*(YHeight/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+2,Origin+Y*(YHeight/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+3,Origin-X*(XHeight/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+4,Origin+X*(XHeight/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+5,Origin-Z*(ZHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,jawIndex+6,Origin+Z*(ZHeight/2.0),Z);

  // Walls 
  ModelSupport::buildPlane(SMap,jawIndex+13,Origin-X*(wallThick+XHeight/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+14,Origin+X*(wallThick+XHeight/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+15,Origin-Z*(wallThick+ZHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,jawIndex+16,Origin+Z*(wallThick+ZHeight/2.0),Z);

  // X JAW : [+ve Y side]
  
  ModelSupport::buildPlane(SMap,jawIndex+101,Origin+Y*(jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+102,Origin+Y*(xThick+jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+103,Origin-X*(xOpen/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+104,Origin+X*(xOpen/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+153,Origin-X*(xLen+xOpen/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+154,Origin+X*(xLen+xOpen/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+105,Origin-Z*(xCross/2.0),Z);
  ModelSupport::buildPlane(SMap,jawIndex+106,Origin+Z*(xCross/2.0),Z);

  // Z JAW : [-ve Y Side]
  
  ModelSupport::buildPlane(SMap,jawIndex+201,Origin-Y*(zThick+jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+202,Origin-Y*(jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+203,Origin-X*(zCross/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+204,Origin+X*(zCross/2.0),X);

  ModelSupport::buildPlane(SMap,jawIndex+205,Origin-Z*(zOpen/2.0),Z);
  ModelSupport::buildPlane(SMap,jawIndex+206,Origin+Z*(zOpen/2.0),Z);
  ModelSupport::buildPlane(SMap,jawIndex+255,Origin-Z*(zLen+zOpen/2.0),Z);
  ModelSupport::buildPlane(SMap,jawIndex+256,Origin+Z*(zLen+zOpen/2.0),Z);

  
  
  return;
}

void
Jaws::createObjects(Simulation& System)
  /*!
    Adds the zoom chopper box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Jaws","createObjects");


  HeadRule CutRule;
  std::string Out;

  // X Jaw A: 
  Out=ModelSupport::getComposite(SMap,jawIndex," 101 -102 -103 153 105 -106 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,xJawMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,jawIndex," 101 -102 103 -104 105 -106 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,jawIndex," 101 -102 104 -154 105 -106 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,xJawMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,jawIndex," 101 -102 153 -154 105 -106 ");
  CutRule.procString(Out);

  // Z Jaw A: 
  Out=ModelSupport::getComposite(SMap,jawIndex," 201 -202 -205 255 203 -204 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,zJawMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,jawIndex," 201 -202 205 -206 203 -204 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,jawIndex," 201 -202 206 -256 203 -204 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,zJawMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,jawIndex," 201 -202 203 -204 255 -256 ");
  CutRule.addUnion(Out);
  CutRule.makeComplement();

  Out=ModelSupport::getComposite(SMap,jawIndex,"1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+CutRule.display()));

  Out=ModelSupport::getComposite(SMap,jawIndex,
				 "1 -2 13 -14 15 -16 (-3 : 4 : -5: 6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  
  Out=ModelSupport::getComposite(SMap,jawIndex," 1 -2 13 -14 15 -16 ");
  addOuterSurf(Out);      

  return;
}

void
Jaws::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Jaws","createLinks");

  FixedComp::setConnect(0,Origin-Y*(YHeight/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(jawIndex+1));

  FixedComp::setConnect(1,Origin+Y*(YHeight/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(jawIndex+2));

  FixedComp::setConnect(2,Origin-X*(wallThick+XHeight/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(jawIndex+13));
 
  FixedComp::setConnect(3,Origin+X*(wallThick+XHeight/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(jawIndex+14));

  FixedComp::setConnect(4,Origin-Z*(wallThick+ZHeight/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(jawIndex+15));

  FixedComp::setConnect(5,Origin+Z*(wallThick+ZHeight/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(jawIndex+16));

  return;
}

void
Jaws::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Jaws","createAll(FC)");
  
  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

  
}  // NAMESPACE constructSystem
