/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/TSMainBuilding.cxx
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
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

#include "TSMainBuilding.h"

namespace essSystem
{

TSMainBuilding::TSMainBuilding(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,7),
  attachSystem::CellMap(),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TSMainBuilding::TSMainBuilding(const TSMainBuilding& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),
  mainMat(A.mainMat),roomMat(A.roomMat)
  /*!
    Copy constructor
    \param A :: TSMainBuilding to copy
  */
{}

TSMainBuilding&
TSMainBuilding::operator=(const TSMainBuilding& A)
  /*!
    Assignment operator
    \param A :: TSMainBuilding to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      mainMat=A.mainMat;
      roomMat=A.roomMat;
    }
  return *this;
}

TSMainBuilding::~TSMainBuilding() 
  /*!
    Destructor
  */
{}

void
TSMainBuilding::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("TSMainBuilding","populate");
  
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  roomMat=ModelSupport::EvalMat<int>(Control,keyName+"RoomMat");

  return;
}
  
void
TSMainBuilding::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("TSMainBuilding","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}
  
void
TSMainBuilding::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TSMainBuilding","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(depth),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*(height),Z);
  
  return;
}
  
void
TSMainBuilding::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TSMainBuilding","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,Out));
  //  setCell("main",cellIndex-1);

  addOuterSurf(Out);

  return;
}

  
void
TSMainBuilding::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("TSMainBuilding","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));
  
  FixedComp::setConnect(1,Origin+Y*(length/2),Y);
  FixedComp::setLinkSurf(1, SMap.realSurf(surfIndex+2));

  FixedComp::setConnect(2,Origin-X*(width/2),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(surfIndex+3));
  
  FixedComp::setConnect(3,Origin+X*(width/2),X);
  FixedComp::setLinkSurf(3, SMap.realSurf(surfIndex+4));

  FixedComp::setConnect(4,Origin-Z*(depth),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(surfIndex+5));
  
  FixedComp::setConnect(5,Origin+X*(height),Z);
  FixedComp::setLinkSurf(6, SMap.realSurf(surfIndex+6));

  return;
}
  
  

  
void
TSMainBuilding::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("TSMainBuilding","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);              

  return;
}

}  // essSystem essSystem
