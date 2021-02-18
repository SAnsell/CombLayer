/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/MagRoundVacuumPipe.cxx
 *
 * Copyright (c) 2017-2021 by Konstantin Batkov
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "MagRoundVacuumPipe.h"

namespace essSystem
{

MagRoundVacuumPipe::MagRoundVacuumPipe
  (const std::string& Base,const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+std::to_string(Index),6),
  attachSystem::CellMap(),
  baseName(Base),extraName(Base+Key),voidMat(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MagRoundVacuumPipe::MagRoundVacuumPipe(const MagRoundVacuumPipe& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  length(A.length),
  pipeRad(A.pipeRad),
  thickness(A.thickness),
  voidMat(A.voidMat),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: MagRoundVacuumPipe to copy
  */
{}

MagRoundVacuumPipe&
MagRoundVacuumPipe::operator=(const MagRoundVacuumPipe& A)
  /*!
    Assignment operator
    \param A :: MagRoundVacuumPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      mat=A.mat;
      voidMat=A.voidMat;
      thickness=A.thickness;
      pipeRad=A.pipeRad;
    }
  return *this;
}

MagRoundVacuumPipe*
MagRoundVacuumPipe::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new MagRoundVacuumPipe(*this);
}

MagRoundVacuumPipe::~MagRoundVacuumPipe()
  /*!
    Destructor
  */
{}

void
MagRoundVacuumPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MagRoundVacuumPipe","populate");

  FixedOffset::populate(Control);

  length=Control.EvalTail<double>(keyName,extraName,"Length");
  thickness=Control.EvalTail<double>(keyName,extraName,"Thickness");
  pipeRad=Control.EvalTail<double>(keyName,extraName,"InnerRadius");
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);

  //waterMat=ModelSupport::EvalMat<int>(Control,baseName+"WaterMat");

  return;
}

void
MagRoundVacuumPipe::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MagRoundVacuumPipe","createSurfaces");

  // Dividers
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Y,pipeRad);
  ModelSupport::buildCylinder(SMap,buildIndex+9,Origin,Y,pipeRad+thickness);

  return;
}

void
MagRoundVacuumPipe::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MagRoundVacuumPipe","createObjects");

  HeadRule HR;

  ELog::EM<<"OBJ == "<<keyName<<":"<<voidMat<<ELog::endDiag;
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 -8");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 8 -9");
  makeCell("Pipe",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 -9");
  addOuterSurf(HR);
  
  return;
}

void
MagRoundVacuumPipe::createLinks()

  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("MagRoundVacuumPipe","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  return;
}




void
MagRoundVacuumPipe::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("MagRoundVacuumPipe","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
