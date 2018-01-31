/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/JawUnit.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"

#include "JawUnit.h"

namespace constructSystem
{

JawUnit::JawUnit(const std::string& Key) : 
  attachSystem::FixedOffset(Key,10),attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  jawIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(jawIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

JawUnit::JawUnit(const JawUnit& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  jawIndex(A.jawIndex),cellIndex(A.cellIndex),zOpen(A.zOpen),
  zOffset(A.zOffset),zThick(A.zThick),zHeight(A.zHeight),
  zWidth(A.zWidth),xOpen(A.xOpen),xOffset(A.xOffset),
  xThick(A.xThick),xHeight(A.xHeight),xWidth(A.xWidth),
  jawGap(A.jawGap),zJawMat(A.zJawMat),xJawMat(A.xJawMat)
  /*!
    Copy constructor
    \param A :: JawUnit to copy
  */
{}

JawUnit&
JawUnit::operator=(const JawUnit& A)
  /*!
    Assignment operator
    \param A :: JawUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      zOpen=A.zOpen;
      zOffset=A.zOffset;
      zThick=A.zThick;
      zHeight=A.zHeight;
      zWidth=A.zWidth;
      xOpen=A.xOpen;
      xOffset=A.xOffset;
      xThick=A.xThick;
      xHeight=A.xHeight;
      xWidth=A.xWidth;
      jawGap=A.jawGap;
      zJawMat=A.zJawMat;
      xJawMat=A.xJawMat;
    }
  return *this;
}

  
JawUnit::~JawUnit() 
  /*!
    Destructor
  */
{}


void
JawUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("JawUnit","populate");

  FixedOffset::populate(Control);
  
  zOpen=Control.EvalVar<double>(keyName+"ZOpen");
  zOffset=Control.EvalVar<double>(keyName+"ZOffset");
  zThick=Control.EvalVar<double>(keyName+"ZThick");
  zHeight=Control.EvalVar<double>(keyName+"ZHeight");
  zWidth=Control.EvalVar<double>(keyName+"ZWidth");

  xOpen=Control.EvalVar<double>(keyName+"XOpen");
  xOffset=Control.EvalVar<double>(keyName+"XOffset");
  xThick=Control.EvalVar<double>(keyName+"XThick");
  xHeight=Control.EvalVar<double>(keyName+"XHeight");
  xWidth=Control.EvalVar<double>(keyName+"XWidth");

  jawGap=Control.EvalVar<double>(keyName+"Gap");

  zJawMat=ModelSupport::EvalMat<int>(Control,keyName+"ZJawMat",
				     keyName+"JawMat");
  xJawMat=ModelSupport::EvalMat<int>(Control,keyName+"XJawMat",
				     keyName+"JawMat");
  
  return;
}

void
JawUnit::createUnitVector(const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("JawUnit","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  FixedOffset::applyOffset();

  return;
}

void
JawUnit::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("JawUnit","createSurfaces");
  
  ModelSupport::buildPlane(SMap,jawIndex+101,Origin+Y*(jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+102,Origin+Y*(xThick+jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+103,Origin-X*
			   (xWidth+xOpen/2.0-xOffset),X);
  ModelSupport::buildPlane(SMap,jawIndex+104,Origin-
			   X*(xOpen/2.0-xOffset),X);

  ModelSupport::buildPlane(SMap,jawIndex+153,Origin+X*(xOpen/2.0+xOffset),X);
  ModelSupport::buildPlane(SMap,jawIndex+154,Origin+
			   X*(xWidth+xOpen/2.0+xOffset),X);

  ModelSupport::buildPlane(SMap,jawIndex+105,Origin-Z*(xHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,jawIndex+106,Origin+Z*(xHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,jawIndex+201,Origin-Y*(zThick+jawGap/2.0),Y);
  ModelSupport::buildPlane(SMap,jawIndex+202,Origin-Y*(jawGap/2.0),Y);

  ModelSupport::buildPlane(SMap,jawIndex+203,Origin-X*(zWidth/2.0),X);
  ModelSupport::buildPlane(SMap,jawIndex+204,Origin+X*(zWidth/2.0),X);

  ModelSupport::buildPlane(SMap,jawIndex+205,Origin-
			   Z*(zHeight+zOpen/2.0-zOffset),Z);
  ModelSupport::buildPlane(SMap,jawIndex+206,Origin-
			   Z*(zOpen/2.0-zOffset),Z);

  ModelSupport::buildPlane(SMap,jawIndex+255,Origin+Z*(zOpen/2.0+zOffset),Z);
  ModelSupport::buildPlane(SMap,jawIndex+256,Origin+
			   Z*(zHeight+zOpen/2.0+zOffset),Z);

  return;
}

void
JawUnit::createObjects(Simulation& System)
  /*!
    Adds the zoom chopper box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("JawUnit","createObjects");
  std::string Out;

  Out=ModelSupport::getComposite(SMap,jawIndex," 101 -102 103 -104 105 -106 ");
  makeCell("xJaw",System,cellIndex++,xJawMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,jawIndex," 101 -102 153 -154 105 -106 ");
  makeCell("xJaw",System,cellIndex++,xJawMat,0.0,Out);
  // gap
  Out=ModelSupport::getComposite(SMap,jawIndex,"101 -102 104 -153 105 -106");
  makeCell("xGap",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,jawIndex," 201 -202 203 -204 205 -206 ");
  makeCell("zJaw",System,cellIndex++,zJawMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,jawIndex," 201 -202 203 -204 255 -256 ");
  makeCell("zJaw",System,cellIndex++,zJawMat,0.0,Out);
  // gap
  Out=ModelSupport::getComposite(SMap,jawIndex,"201 -202 203 -204 206 -255 ");
  makeCell("zGap",System,cellIndex++,0,0.0,Out);

   
  Out=ModelSupport::getComposite(SMap,jawIndex," 101 -102 103 -154 105 -106 ");
  addOuterSurf(Out);      

  Out=ModelSupport::getComposite(SMap,jawIndex," 201 -202 203 -204 205 -256 ");
  addOuterUnionSurf(Out);      

  return;
}

void
JawUnit::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("JawUnit","createLinks");

  FixedComp::setConnect(0,Origin-Y*(zThick+jawGap/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(jawIndex+201));
  
  FixedComp::setConnect(1,Origin+Y*(xThick+jawGap/2.0),-Y);
  FixedComp::setLinkSurf(1,-SMap.realSurf(jawIndex+102));

  // x values (4):
  FixedComp::setConnect(2,Origin-X*(xOpen/2.0-xOffset),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(jawIndex+103));

  FixedComp::setConnect(3,Origin+X*(xOpen/2.0+xOffset),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(jawIndex+154));

  FixedComp::setConnect(4,Origin-Z*(xHeight/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(jawIndex+105));

  FixedComp::setConnect(5,Origin+Z*(xHeight/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(jawIndex+106));

  // z values (4):
  FixedComp::setConnect(6,Origin-X*(xWidth/2.0),-X);
  FixedComp::setLinkSurf(6,-SMap.realSurf(jawIndex+203));

  FixedComp::setConnect(7,Origin+X*(xWidth/2.0),X);
  FixedComp::setLinkSurf(7,SMap.realSurf(jawIndex+204));

  FixedComp::setConnect(8,Origin-Z*(zOpen/2.0-zOffset),-Z);
  FixedComp::setLinkSurf(8,-SMap.realSurf(jawIndex+205));

  FixedComp::setConnect(9,Origin+Z*(zOpen/2.0+zOffset),Z);
  FixedComp::setLinkSurf(9,SMap.realSurf(jawIndex+256));

  return;
}
  
void
JawUnit::createAll(Simulation& System,
		const attachSystem::FixedComp& FC,
		const long int FIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("JawUnit","createAll(FC)");
  
  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

  
}  // NAMESPACE constructSystem
