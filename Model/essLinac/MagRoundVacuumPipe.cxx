/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/MagRoundVacuumPipe.cxx
 *
 * Copyright (c) 2017-2018 by Konstantin Batkov
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
#include "groupRange.h"
#include "objectGroups.h"
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
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"
#include "CellMap.h"
#include "MagRoundVacuumPipe.h"

namespace essSystem
{

  MagRoundVacuumPipe::MagRoundVacuumPipe(const std::string& Base,const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+StrFunc::makeString(Index),6),
  attachSystem::CellMap(),
  baseName(Base),
  extraName(Base+Key),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MagRoundVacuumPipe::MagRoundVacuumPipe(const MagRoundVacuumPipe& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  length(A.length),
  pipeRad(A.pipeRad),
  mat(A.mat),
  voidMat(A.voidMat),
  thickness(A.thickness)

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
      engActive=A.engActive;
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
  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  length=Control.EvalPair<double>(keyName,extraName,"Length");
  voidMat=Control.EvalPair<int>(keyName,extraName,"VoidMat");
  thickness=Control.EvalPair<double>(keyName,extraName,"Thickness");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  pipeRad=Control.EvalPair<double>(keyName,extraName,"InnerRadius");

  //waterMat=ModelSupport::EvalMat<int>(Control,baseName+"WaterMat");

  return;
}

void
MagRoundVacuumPipe::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("MagRoundVacuumPipe","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

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
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin,Z);

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildCylinder(SMap,surfIndex+8,Origin,Y,pipeRad);
  ModelSupport::buildCylinder(SMap,surfIndex+9,Origin,Y,pipeRad+thickness);

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

  std::string Out,Side;

  int SI(surfIndex);


  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -8");
  System.addCell(MonteCarlo::Object(cellIndex++,voidMat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 8 -9");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  addOuterUnionSurf(Out);



  
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
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+12));


  // for (int i=6; i<8; i++)
  //   ELog::EM << keyName << " " << i << "\t" << getLinkSurf(i) << "\t" << getLinkPt(i) << "\t\t" << getLinkAxis(i) << ELog::endDiag;

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
