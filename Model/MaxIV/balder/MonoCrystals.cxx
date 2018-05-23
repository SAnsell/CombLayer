/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/MonoCrystals.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "MonoCrystals.h"


namespace xraySystem
{

MonoCrystals::MonoCrystals(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,8),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  xtalIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(xtalIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


MonoCrystals::~MonoCrystals()
  /*!
    Destructor
   */
{}

void
MonoCrystals::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MonoCrystals","populate");

  FixedOffset::populate(Control);

  gap=Control.EvalVar<double>(keyName+"Gap");
  theta=Control.EvalVar<double>(keyName+"Theta");
  phiA=Control.EvalDefVar<double>(keyName+"PhiA",0.0);
  phiB=Control.EvalDefVar<double>(keyName+"PhiB",0.0);
  
  widthA=Control.EvalPair<double>(keyName+"WidthA",keyName+"Width");
  thickA=Control.EvalPair<double>(keyName+"ThickA",keyName+"Thick");
  lengthA=Control.EvalPair<double>(keyName+"LengthA",keyName+"Length");

  widthB=Control.EvalPair<double>(keyName+"WidthB",keyName+"Width");
  thickB=Control.EvalPair<double>(keyName+"ThickB",keyName+"Thick");
  lengthB=Control.EvalPair<double>(keyName+"LengthB",keyName+"Length");
  
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  baseExtra=Control.EvalVar<double>(keyName+"BaseExtra");

  xtalMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");

  return;
}

void
MonoCrystals::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("MonoCrystals","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();  
  return;
}

void
MonoCrystals::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("MonoCrystals","createSurfaces");

  // main xstal CENTRE AT ORIGIN 
  const Geometry::Quaternion QXA
    (Geometry::Quaternion::calcQRotDeg(theta,X));

  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Y);
  Geometry::Vec3D PZ(Z);

  QXA.rotate(PY);
  QXA.rotate(PZ);
  
  ModelSupport::buildPlane(SMap,xtalIndex+101,Origin-PY*(lengthA/2.0),PY);
  ModelSupport::buildPlane(SMap,xtalIndex+102,Origin+PY*(lengthA/2.0),PY);
  ModelSupport::buildPlane(SMap,xtalIndex+103,Origin-PX*(widthA/2.0),PX);
  ModelSupport::buildPlane(SMap,xtalIndex+104,Origin+PX*(widthA/2.0),PX);
  ModelSupport::buildPlane(SMap,xtalIndex+105,Origin-PZ*thickA,PZ);
  ModelSupport::buildPlane(SMap,xtalIndex+106,Origin,PZ);

  const Geometry::Vec3D BOrg=
    Origin+Y*(gap/tan(theta*2.0*M_PI/180.0))+Z*gap;
  
  ModelSupport::buildPlane(SMap,xtalIndex+201,BOrg-PY*(lengthB/2.0),PY);
  ModelSupport::buildPlane(SMap,xtalIndex+202,BOrg+PY*(lengthB/2.0),PY);
  ModelSupport::buildPlane(SMap,xtalIndex+203,BOrg-PX*(widthB/2.0),PX);
  ModelSupport::buildPlane(SMap,xtalIndex+204,BOrg+PX*(widthB/2.0),PX);
  ModelSupport::buildPlane(SMap,xtalIndex+205,BOrg,PZ);
  ModelSupport::buildPlane(SMap,xtalIndex+206,BOrg+PZ*thickB,PZ);
  
  
  return; 
}

void
MonoCrystals::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MonoCrystals","createObjects");

  std::string Out;
  // xstal A
  Out=ModelSupport::getComposite(SMap,xtalIndex," 101 -102 103 -104 105 -106 ");
  makeCell("XtalA",System,cellIndex++,xtalMat,0.0,Out);
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,xtalIndex," 201 -202 203 -204 205 -206 ");
  makeCell("XtalB",System,cellIndex++,xtalMat,0.0,Out);
  addOuterUnionSurf(Out);
  
  return; 
}

void
MonoCrystals::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MonoCrystals","createLinks");
  
  return;
}

void
MonoCrystals::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MonoCrystals","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
