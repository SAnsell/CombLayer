/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/ProtonPipe.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
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
#include "ProtonPipe.h"

namespace bibSystem
{

ProtonPipe::ProtonPipe(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,3)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

ProtonPipe::ProtonPipe(const ProtonPipe& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  radius(A.radius),
  innerWallThick(A.innerWallThick),wallThick(A.wallThick),
  length(A.length),innerLength(A.innerLength),voidMat(A.voidMat),
  innerWallMat(A.innerWallMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: ProtonPipe to copy
  */
{}

ProtonPipe&
ProtonPipe::operator=(const ProtonPipe& A)
  /*!
    Assignment operator
    \param A :: ProtonPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      radius=A.radius;
      innerWallThick=A.innerWallThick;
      wallThick=A.wallThick;
      length=A.length;
      innerLength=A.innerLength;
      voidMat=A.voidMat;
      innerWallMat=A.innerWallMat;
      wallMat=A.wallMat;
    }
  return *this;
}


ProtonPipe::~ProtonPipe()
  /*!
    Destructor
   */
{}

void
ProtonPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("ProtonPipe","populate");

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  radius=Control.EvalVar<double>(keyName+"Radius");
  innerWallThick=Control.EvalVar<double>(keyName+"InnerWallThick");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  length=Control.EvalVar<double>(keyName+"Length");
  innerLength=Control.EvalVar<double>(keyName+"InnerLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  innerWallMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerWallMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
ProtonPipe::createSurfaces(const attachSystem::FixedComp& TarFC,
			   const long int targetIndex)
  /*!
    Create planes for the silicon and Polyethene layers
    \param TarFC :: Target Object
    \param targetIndex :: Target side index 
  */
{
  ELog::RegMethod RegA("ProtonPipe","createSurfaces");

  SMap.addMatch(buildIndex+1,TarFC.getLinkSurf(targetIndex));
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,
			      Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
			      Origin,Y,radius+innerWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,
			      Origin,Y,radius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin+Y*innerLength,Y);

  return; 
}

void
ProtonPipe::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("ProtonPipe","createObjects");
  
  std::string Out;

  // Part without anthing in middle [separated as very long otherwize]xs
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -11 -7");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -2 -7");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // Inner wall
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -11 7 -17");
  System.addCell(MonteCarlo::Object(cellIndex++,innerWallMat,0.0,Out));

  // Outer wall
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -2 7 -27");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -11 -17");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -27 -2");
  addOuterUnionSurf(Out);

  return; 
}

void
ProtonPipe::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("ProtonPipe","createLinks");
  // set Links :: Inner links:
  // Wrapper layer
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin+X*(radius+innerWallThick),X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));

  return;
}

void
ProtonPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& Target,
		      const long int tarIndex)

  /*!
    Extrenal build everything
    \param System :: Simulation
    \param Target :: Target (fixed point for origin via linkPt)
    \param tarIndex :: Target Link Index
   */
{
  ELog::RegMethod RegA("ProtonPipe","createAll");
  populate(System.getDataBase());

  createUnitVector(Target,tarIndex);
  createSurfaces(Target,tarIndex);
  createObjects(System);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
