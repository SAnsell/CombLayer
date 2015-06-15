/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BunkerInsert.cxx
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
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "mergeTemplate.h"

#include "World.h"
#include "BunkerInsert.h"

namespace essSystem
{

BunkerInsert::BunkerInsert(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  insIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(insIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BunkerInsert::~BunkerInsert() 
  /*!
    Destructor
  */
{}

void
BunkerInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BunkerInsert","populate");
  FixedOffset::populate(Control);
  
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  topWall=Control.EvalVar<double>(keyName+"TopWall");
  lowWall=Control.EvalVar<double>(keyName+"LowWall");
  leftWall=Control.EvalVar<double>(keyName+"LeftWall");
  rightWall=Control.EvalVar<double>(keyName+"RightWall");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}
  
void
BunkerInsert::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int orgIndex)
  /*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param FC :: Linked object
    \param orgIndex :: link point
  */
{
  ELog::RegMethod RegA("BunkerInsert","createUnitVector");

  FixedComp::createUnitVector(FC,orgIndex);      
  return;
}
  
void
BunkerInsert::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BunkerInsert","createSurface");

  ModelSupport::buildPlane(SMap,insIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,insIndex+4,Origin+X*width/2.0,X);

  ModelSupport::buildPlane(SMap,insIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,insIndex+6,Origin+Z*height/2.0,Z);

  ModelSupport::buildPlane(SMap,insIndex+13,Origin-X*(width/2.0+leftWall),X);
  ModelSupport::buildPlane(SMap,insIndex+14,Origin+X*(width/2.0+rightWall),X);

  ModelSupport::buildPlane(SMap,insIndex+15,Origin-Z*(height/2.0+lowWall),Z);
  ModelSupport::buildPlane(SMap,insIndex+16,Origin+Z*(height/2.0+topWall),Z);

  return;
}

  
void
BunkerInsert::createObjects(Simulation& System,
			    const std::string& BCell)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param BCell :: Front/back of the bunker + divider
  */
{
  ELog::RegMethod RegA("BunkerInsert","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,insIndex," 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+BCell));
  setCell("void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,insIndex,
				 " 13 -14 15 -16 (-3 : 4: -5: 6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+BCell));
  
  Out=ModelSupport::getComposite(SMap,insIndex," 13 -14 15 -16 ");
  addOuterSurf(Out);
  
  return;
}
  
void
BunkerInsert::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("BunkerInsert","createLinks");


  return;
}


void
BunkerInsert::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int orgIndex,
			const std::string& BWalls)

/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param orgIndex :: link for origin
    \param BWalls :: Bunker wall cut
  */
{
  ELog::RegMethod RegA("BunkerInsert","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,orgIndex);
  createSurfaces();
  createLinks();
  createObjects(System,BWalls);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
