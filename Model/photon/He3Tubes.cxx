/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/He3Tubes.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "He3Tubes.h"


namespace photonSystem
{
      
He3Tubes::He3Tubes(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

He3Tubes::He3Tubes(const He3Tubes& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),  
  nTubes(A.nTubes),
  length(A.length),radius(A.radius),wallThick(A.wallThick),
  gap(A.gap),wallMat(A.wallMat),mat(A.mat)
  /*!
    Copy constructor
    \param A :: He3Tubes to copy
  */
{}

He3Tubes&
He3Tubes::operator=(const He3Tubes& A)
  /*!
    Assignment operator
    \param A :: He3Tubes to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      nTubes=A.nTubes;
      length=A.length;
      radius=A.radius;
      wallThick=A.wallThick;
      gap=A.gap;
      wallMat=A.wallMat;
      mat=A.mat;
    }
  return *this;
}

He3Tubes::~He3Tubes()
  /*!
    Destructor
  */
{}

He3Tubes*
He3Tubes::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new He3Tubes(*this);
}

void
He3Tubes::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("He3Tubes","populate");

  FixedRotate::populate(Control);

  nTubes=Control.EvalVar<size_t>(keyName+"NTubes");
  if (nTubes)
    {
      radius=Control.EvalVar<double>(keyName+"Radius");
      length=Control.EvalVar<double>(keyName+"Length");
      wallThick=Control.EvalVar<double>(keyName+"WallThick");
      gap=Control.EvalVar<double>(keyName+"Gap");
      separation=gap+2*(radius+wallThick);
      wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
      mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
    }

  return;
}

void
He3Tubes::createSurfaces()
  /*!
    Create simple structures
  */
{
  ELog::RegMethod RegA("He3Tubes","createSurfaces");
  
  // boundary surfaces
  const double offset(radius+1.01*wallThick);
  const Geometry::Vec3D LCentPt=
    Origin-X*(offset+separation*(static_cast<double>(nTubes)-1.0)/2.0);
  const Geometry::Vec3D RCentPt=
    Origin+X*(offset+separation*(static_cast<double>(nTubes)-1.0)/2.0);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*offset,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*offset,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,LCentPt,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,RCentPt,X);

  
  ModelSupport::buildPlane
    (SMap,buildIndex+4,
     Origin+X*(offset+separation*(static_cast<double>(nTubes)-1.0)/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(length/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(length/2.0),Z);


  Geometry::Vec3D CentPt=
    Origin-X*(separation*(static_cast<double>(nTubes)-1.0)/2.0);

  int tubeIndex(buildIndex);
  for(size_t i=0;i<nTubes;i++)
    {
      ModelSupport::buildCylinder(SMap,tubeIndex+7,CentPt,Z,radius);
      ModelSupport::buildCylinder(SMap,tubeIndex+17,CentPt,Z,radius+wallThick);
      CentPt+=X*separation;
      tubeIndex+=100;
    }
  return; 
}

void
He3Tubes::createObjects(Simulation& System)
  /*!
    Create the tubed moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("He3Tubes","createObjects");

  HeadRule HR;

  int tubeIndex(buildIndex);
  HeadRule boxHR;
  for(size_t i=0;i<nTubes;i++)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,tubeIndex,"5 -6 -7M");
      makeCell("He",System,cellIndex++,mat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,tubeIndex,"5 -6 7M -17M ");
      makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

      boxHR *= HeadRule(SMap,tubeIndex,17);
      tubeIndex+=100;
    }
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  makeCell("Outer",System,cellIndex++,0,0.0,HR*boxHR);
  
  addOuterSurf(HR);
  return; 
}

void
He3Tubes::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("He3Tubes","createLinks");
  
  FixedComp::setConnect(4,Origin-Z*(length/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(length/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}
  
void
He3Tubes::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("He3Tubes","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       


  return;
}

}  // NAMESPACE photonSystem
