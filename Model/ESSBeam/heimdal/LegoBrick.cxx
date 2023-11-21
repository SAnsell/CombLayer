/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   heimdal/LegoBrick.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"

#include "LegoBrick.h"

namespace essSystem
{

LegoBrick::LegoBrick(const std::string& Key)  :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LegoBrick::LegoBrick(const LegoBrick& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  width(A.width),height(A.height),
  depth(A.depth),mat(A.mat)
  /*!
    Copy constructor
    \param A :: LegoBrick to copy
  */
{}

LegoBrick&
LegoBrick::operator=(const LegoBrick& A)
  /*!
    Assignment operator
    \param A :: LegoBrick to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      width=A.width;
      height=A.height;
      depth=A.depth;
      mat=A.mat;
    }
  return *this;
}


LegoBrick::~LegoBrick() 
  /*!
    Destructor
  */
{}


void
LegoBrick::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("LegoBrick","populate");
  
  FixedRotate::populate(Control);

  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  return;
}
  
void
LegoBrick::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("LegoBrick","createSurfaces");
    
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(depth/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(depth/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
  
  return;
}

void 
LegoBrick::createObjects(Simulation& System)
  /*!
    Create a window object
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("LegoBrick","createObjects");
  HeadRule HR;

  //  std::ostringstream cx;

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 3 -4 5 -6");

  System.addCell(cellIndex++,mat,0.0,HR);  

  // here is where we difine the outer box/boundary:
  addOuterSurf(HR);
  return;
}

void
LegoBrick::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("LegoBrick","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System

