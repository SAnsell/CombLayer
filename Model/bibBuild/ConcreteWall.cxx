/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/ConcreteWall.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
// #include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"

#include "ConcreteWall.h"

namespace bibSystem
{

ConcreteWall::ConcreteWall(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ConcreteWall::ConcreteWall(const ConcreteWall& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::ExternalCut(A),
  innerRadius(A.innerRadius),
  thickness(A.thickness),height(A.height),base(A.base),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: ConcreteWall to copy
  */
{}

ConcreteWall&
ConcreteWall::operator=(const ConcreteWall& A)
  /*!
    Assignment operator
    \param A :: ConcreteWall to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      innerRadius=A.innerRadius;
      thickness=A.thickness;
      height=A.height;
      base=A.base;
      mat=A.mat;
    }
  return *this;
}


ConcreteWall::~ConcreteWall()
  /*!
    Destructor
  */
{}

void
ConcreteWall::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("ConcreteWall","populate");
  
  FixedOffset::populate(Control);

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  thickness=Control.EvalVar<double>(keyName+"WallThick");
  height=Control.EvalVar<double>(keyName+"WallHeight");
  base=Control.EvalVar<double>(keyName+"Base");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
    
void
ConcreteWall::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ConcreteWall","createSurface");
  // rotation of axis:
  const int SNouter=ExternalCut::getRule("Outer").getPrimarySurface();
  const int SNfront=ExternalCut::getRule("Front").getPrimarySurface();
  const int SNback=ExternalCut::getRule("Back").getPrimarySurface();

  SMap.addMatch(buildIndex+7,SNouter);
  SMap.addMatch(buildIndex+1,SNfront);
  SMap.addMatch(buildIndex+2,SNback);

  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,innerRadius+thickness);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*base,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);



  return;
}

void
ConcreteWall::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ConcreteWall","createObjects");

  std::string Out;
    
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 5 -6");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"17 -27 5 -6");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));

  // Inner core separte to avoid need to track by guides
  Out=ModelSupport::getComposite(SMap,buildIndex,"-7 5 -6 (-1:-2)");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"-27 5 -6"); 
  addOuterSurf(Out);
  addBoundarySurf(Out);
  
  return;
}

void
ConcreteWall::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("ConcreteWall","createLinks");
  // set Links :: Inner links:

  FixedComp::setConnect(0,Origin+Y*innerRadius,Z);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+17));

  FixedComp::setConnect(1,Origin+Y*(innerRadius+thickness),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+27));

  FixedComp::setConnect(2,Origin-Z*base,-Z);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(3,Origin+Z*height,Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+6));

  return;
}

void
ConcreteWall::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
/*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: inner surface 
   */
{
  ELog::RegMethod RegA("ConcreteWall","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE bibSystem
