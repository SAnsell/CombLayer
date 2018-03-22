/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/Mirror.cxx
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
#include "Mirror.h"


namespace xraySystem
{

Mirror::Mirror(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,8),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  mirrIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(mirrIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


Mirror::~Mirror()
  /*!
    Destructor
   */
{}

void
Mirror::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("Mirror","populate");

  FixedOffset::populate(Control);

  theta=Control.EvalVar<double>(keyName+"Theta");
  phi=Control.EvalDefVar<double>(keyName+"Phi",0.0);
  
  radius=Control.EvalDefVar<double>(keyName+"Radius",0.0);
  width=Control.EvalVar<double>(keyName+"Width");
  thick=Control.EvalVar<double>(keyName+"Thick");
  length=Control.EvalVar<double>(keyName+"Length");
  
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  baseExtra=Control.EvalVar<double>(keyName+"BaseExtra");

  mirrMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");

  return;
}

void
Mirror::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("Mirror","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();  
  return;
}

void
Mirror::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("Mirror","createSurfaces");

  // main xstal CENTRE AT ORIGIN 
  const Geometry::Quaternion QXA
    (Geometry::Quaternion::calcQRotDeg(-theta,X));

  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Y);
  Geometry::Vec3D PZ(Z);

  QXA.rotate(PY);
  QXA.rotate(PZ);
  
  ModelSupport::buildPlane(SMap,mirrIndex+101,Origin-PY*(length/2.0),PY);
  ModelSupport::buildPlane(SMap,mirrIndex+102,Origin+PY*(length/2.0),PY);
  ModelSupport::buildPlane(SMap,mirrIndex+103,Origin-PX*(width/2.0),PX);
  ModelSupport::buildPlane(SMap,mirrIndex+104,Origin+PX*(width/2.0),PX);
  ModelSupport::buildPlane(SMap,mirrIndex+105,Origin-PZ*thick,PZ);
  ModelSupport::buildPlane(SMap,mirrIndex+106,Origin,PZ);

   
  return; 
}

void
Mirror::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("Mirror","createObjects");

  std::string Out;
  // xstal 
  Out=ModelSupport::getComposite(SMap,mirrIndex," 101 -102 103 -104 105 -106 ");
  makeCell("Mirror",System,cellIndex++,mirrMat,0.0,Out);
  addOuterSurf(Out);
  
  return; 
}

void
Mirror::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("Mirror","createLinks");
  
  return;
}

void
Mirror::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("Mirror","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
