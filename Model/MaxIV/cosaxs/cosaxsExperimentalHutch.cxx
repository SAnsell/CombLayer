/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Model/MaxIV/cosaxs/cosaxsExperimentalHutch.cxx
 *
 * Copyright (c) 2004-2019 by Konstantin Batkov
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "ExperimentalHutch.h"
#include "cosaxsExperimentalHutch.h"

namespace xraySystem
{

cosaxsExperimentalHutch::cosaxsExperimentalHutch(const std::string& Key) :
  xraySystem::ExperimentalHutch(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

cosaxsExperimentalHutch::~cosaxsExperimentalHutch() 
  /*!
    Destructor
  */
{}

void
cosaxsExperimentalHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("cosaxsExperimentalHutch","populate");

  backHoleXStep=Control.EvalDefVar<double>(keyName+"BackHoleXStep",0.0);
  backHoleZStep=Control.EvalDefVar<double>(keyName+"BackHoleZStep",0.0);
  backHoleRadius=Control.EvalDefVar<double>(keyName+"BackHoleRadius",0.0);

  return;
}
 
void
cosaxsExperimentalHutch::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("cosaxsExperimentalHutch","createSurfaces");


  if (backHoleRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder
      (SMap,buildIndex+7,Origin+X*backHoleXStep+Z*backHoleZStep,Y,backHoleRadius);
  
  return;
}

void
cosaxsExperimentalHutch::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("cosaxsExperimentalHutch","createObjects");

  // HeadRule A(SMap.realSurf(buildIndex+7));
  // for(const std::string& layer : {"Inner","Lead","Outer"})
  //     CellMap::insertComponent(System,layer+"BackWall",A);

  return;
}

void
cosaxsExperimentalHutch::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("cosaxsExperimentalHutch","createAll(FC)");

  xraySystem::ExperimentalHutch::createAll(System,FC,FIndex);
  populate(System.getDataBase());

  createSurfaces();    
  createObjects(System);
  
  return;
}
  
}  // NAMESPACE xraySystem
