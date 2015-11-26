/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Bunker.cxx
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
#include "BaseMap.h"
#include "CellMap.h"

#include "World.h"
#include "Bunker.h"
#include "RoofPillars.h"


namespace essSystem
{

RoofPillars::RoofPillars(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  attachSystem::CellMap(),
  pillarIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bnkIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RoofPillars::~RoofPillars() 
  /*!
    Destructor
  */
{}

void
RoofPillars::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("RoofPillars","populate");

  radius=Control.EvalVar<double>(keyName+"Radius");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
  
void
RoofPillars::createUnitVector(const attachSystem::FixedComp& MainCentre,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param sideIndex :: Side for linkage centre
  */
{
  ELog::RegMethod RegA("RoofPillars","createUnitVector");

  FixedComp::createUnitVector(MainCentre,sideIndex);      
  return;
}

  
void
RoofPillars::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RoofPillars","createSurface");

  int RI(rodIndex);
  for(const Geomtery::Vec3D& Pt : CentPoint)
    {
      ModelSupport::buildCylinder(SMap,RI+7,Origin+CentPoint,Z,radius);
    }

  return;
}


  
  
void
RoofPillars::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RoofPillars","createObjects");

  std::string Out;

  int RI(rodIndex);
  for(const size_t i=0;i<CentPoint.size();i++)
    {
      Out=ModelSupport::getComposite(SMap,rodIndex,RI,"1 -2 -7M");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
      addCell("Pillar",cellIndex-1);
      RI+=10;
    }
        
  return;
}



  
void
RoofPillars::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("RoofPillars","createLinks");

  return;
}


  
void
RoofPillars::createAll(Simulation& System,
		     const Bunker& bunkerObj)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param bunkerObj :: Bunker object 
  */
{
  ELog::RegMethod RegA("RoofPillars","createAll");

  populate(System.getDataBase());
  createUnitVector(MainCentre,FC,linkIndex);
  createSurfaces();
  createObjects(System,FC,linkIndex);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
