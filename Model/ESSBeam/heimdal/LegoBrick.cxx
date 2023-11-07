/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   heimdal/LegoBrick.cxx
 *
 * Modified: Isabel Llamas-Jansa, November 2023 
 * - added comments and separation lines for self
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
#include "MaterialSupport.h"
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
    This is a Constructor, with 3 geometry item classes from the
    attachSystem namespace  
    Here, ALL variables are left unpopulated.
    \param Key :: Name for each item constructed; usful for searching 
  */
{}

LegoBrick::LegoBrick(const LegoBrick& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  width(A.width),height(A.height),
  depth(A.depth),mat(A.mat)
  /*!
    This is a copy of the constructor above, with A as key.
    Added some parameters for the brick: width, height, depth and material.
    \param A :: LegoBrick to copy; each copy of the brick is identified by parameter A
  */
{}

LegoBrick&
LegoBrick::operator=(const LegoBrick& A)
  /*!
    Assignment operator as function of the LegoBrick
    variable and the parameter A
    If the condition: this is not equal to the A pointer,
    the operator assigns the values and returns
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
    deallocates the memory reserved for this item
  */
{}

void LegoBrick::populate(const FuncDataBase& Control)
  /*!
    Creates some empty variables and Populates them using the
    parameter Control
    \param Control :: DataBase of variables
    For the brick box: FixedRotate, depth(y-axis), height(z-axis),
    width (x-axis)
  */

{
  ELog::RegMethod RegA("LegoBrick","populate");
  
  FixedRotate::populate(Control);

  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void LegoBrick::createSurfaces()
  /*!
    Creates empty surfaces and then makes them based on the parameters below
    For the brick box, the surfaces are flat planes defined by SMap (pointer to real objects), an index (to identify each surface), 
    their position with respect to the origin and the axis they are perpendicular to.
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

void LegoBrick::createObjects(Simulation& System)
  /*!
    Creates a brick object/cell that can be used in simulation. The material is included here too.
    Also, it creates an outer boundary around the box to account for irregularities on the brick's surfaces.
    \param System :: Simulation    
  */
{
  ELog::RegMethod RegA("LegoBrick","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  
  // here, it makes a box/cell out of the planes defined above

  System.addCell(cellIndex++,mat,0.0,HR);  

  // here is where we define the outer box/boundary:
  addOuterSurf(HR);

  return;
}

void LegoBrick::createAll(Simulation& System, 
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    This function creates eveything needed for the simulation.
    \param System :: This is the system that runs the simulation in which
    we are adding the brick
    \param FC :: This is a fixed Component to place our object within;
      it is the central origin
      \param sideIdex :: this is a link point to the origin
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

}  // NAMESPACE essSystem
