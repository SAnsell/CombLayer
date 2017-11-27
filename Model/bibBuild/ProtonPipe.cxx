/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/ProtonPipe.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ProtonPipe.h"

namespace bibSystem
{

ProtonPipe::ProtonPipe(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,3),
  protonIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(protonIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

ProtonPipe::ProtonPipe(const ProtonPipe& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  protonIndex(A.protonIndex),cellIndex(A.cellIndex),
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
      cellIndex=A.cellIndex;
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
ProtonPipe::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("ProtonPipe","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
ProtonPipe::createSurfaces(const attachSystem::FixedComp& TarFC,
			   const long int targetIndex)
  /*!
    Create planes for the silicon and Polyethene layers
    \param TarFC :: Target Object
    \param tarIndex :: Target side index 
  */
{
  ELog::RegMethod RegA("ProtonPipe","createSurfaces");

  SMap.addMatch(protonIndex+1,TarFC.getLinkSurf(targetIndex));
  ModelSupport::buildPlane(SMap,protonIndex+2,Origin+Y*length,Y);

  ModelSupport::buildCylinder(SMap,protonIndex+7,
			      Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,protonIndex+17,
			      Origin,Y,radius+innerWallThick);
  ModelSupport::buildCylinder(SMap,protonIndex+27,
			      Origin,Y,radius+wallThick);

  ModelSupport::buildPlane(SMap,protonIndex+11,
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
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -11 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"11 -2 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Inner wall
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -11 7 -17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,0.0,Out));

  // Outer wall
  Out=ModelSupport::getComposite(SMap,protonIndex,"11 -2 7 -27");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -11 -17");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,protonIndex,"11 -27 -2");
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
  FixedComp::setLinkSurf(0,-SMap.realSurf(protonIndex+1));

  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(protonIndex+2));

  FixedComp::setConnect(2,Origin+X*(radius+innerWallThick),X);
  FixedComp::setLinkSurf(2,SMap.realSurf(protonIndex+17));

  return;
}

void
ProtonPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& Target,
		      const size_t tarIndex)

  /*!
    Extrenal build everything
    \param System :: Simulation
    \param Target :: Target (fixed point for origin via linkPt)
    \param tarIndex :: Target Link Index
   */
{
  ELog::RegMethod RegA("ProtonPipe","createAll");
  populate(System.getDataBase());

  createUnitVector(Target,tarIndex+1);
  createSurfaces(Target,tarIndex+1);
  createObjects(System);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
