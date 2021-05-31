/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ChopperHousing.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
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
#include "BaseMap.h"
#include "CellMap.h"

#include "ChopperHousing.h"

namespace constructSystem
{

ChopperHousing::ChopperHousing(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ChopperHousing::ChopperHousing(const ChopperHousing& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  voidHeight(A.voidHeight),voidWidth(A.voidWidth),
  voidDepth(A.voidDepth),voidThick(A.voidThick),
  wallThick(A.wallThick),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: ChopperHousing to copy
  */
{}

ChopperHousing&
ChopperHousing::operator=(const ChopperHousing& A)
  /*!
    Assignment operator
    \param A :: ChopperHousing to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      voidHeight=A.voidHeight;
      voidWidth=A.voidWidth;
      voidDepth=A.voidDepth;
      voidThick=A.voidThick;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
    }
  return *this;
}


ChopperHousing::~ChopperHousing() 
  /*!
    Destructor
  */
{}

void
ChopperHousing::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ChopperHousing","populate");

  FixedOffset::populate(Control);
  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidThick=Control.EvalVar<double>(keyName+"VoidThick");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");  
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
ChopperHousing::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("ChopperHousing","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
ChopperHousing::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ChopperHousing","createSurfaces");


  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);  

  // Wall system [front face is link surf]
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(wallThick+voidThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(wallThick+voidThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(wallThick+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(wallThick+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(voidHeight+wallThick),Z);
  
  return;
}

void
ChopperHousing::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ChopperHousing","createObjects");

  std::string Out;

  // Void 
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  // Wall
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -12 13 -14 5 -16 (-1:2:-3:4:6)");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  addCell("Wall",cellIndex-1);

  // Outer
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 13 -14 5 -16");
  addOuterSurf(Out);      
  return;
}

void
ChopperHousing::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ChopperHousing","createLinks");

  FixedComp::setConnect(0,Origin-Y*(voidThick/2.0+wallThick),-Y);
  FixedComp::setConnect(1,Origin+Y*(voidThick/2.0+wallThick),Y);
  FixedComp::setConnect(2,Origin-X*(voidWidth/2.0+wallThick),-X);
  FixedComp::setConnect(3,Origin+X*(voidWidth/2.0+wallThick),X);
  FixedComp::setConnect(4,Origin-Y*(voidDepth+wallThick),-Z);
  FixedComp::setConnect(5,Origin-Y*(voidHeight+wallThick),Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));
  
  return;
}

void
ChopperHousing::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("ChopperHousing","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem
