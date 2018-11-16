/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sinbadBuild/Cave.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"

#include "Cave.h"

namespace sinbadSystem
{

Cave::Cave(const std::string& Key) : 
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Cave::~Cave() 
  /*!
    Destructor
  */
{}


void
Cave::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("Cave","populate");

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}


void
Cave::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComponent for origin
  */
{
  ELog::RegMethod RegA("Cave","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);

 return;
}


void
Cave::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("Cave","createSurface");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(wallThick+height/2.0),Z);
  
  return;
}


void
Cave::createObjects(Simulation& System)
  /*!
    Create all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Cave","createObjects");
  std::string Out;
   
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6  ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));  

  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 3 -4 5 -6 (-11:12:-13:14:-15:16) ");  

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf(Out);
  
  return;
}


void
Cave::createAll(Simulation& System,
			const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("Cave","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  insertObjects(System);

  return;
}
  
}   // NAMESPACE sinbadSystem
