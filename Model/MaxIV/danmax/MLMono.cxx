/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmax/MLMono.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "MLMono.h"


namespace xraySystem
{

MLMono::MLMono(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,8),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}



MLMono::~MLMono()
  /*!
    Destructor
   */
{}

void
MLMono::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MLMono","populate");

  FixedRotate::populate(Control);

  gap=Control.EvalVar<double>(keyName+"Gap");
  thetaA=Control.EvalVar<double>(keyName+"ThetaA");
  thetaB=Control.EvalVar<double>(keyName+"ThetaB");
  
  widthA=Control.EvalVar<double>(keyName+"WidthA");
  heightA=Control.EvalVar<double>(keyName+"HeightA");
  lengthA=Control.EvalVar<double>(keyName+"LengthA");

  widthB=Control.EvalVar<double>(keyName+"WidthB");
  heightB=Control.EvalVar<double>(keyName+"HeightB");
  lengthB=Control.EvalVar<double>(keyName+"LengthB");

  supportAGap=Control.EvalVar<double>(keyName+"SupportAGap");
  supportAExtra=Control.EvalVar<double>(keyName+"SupportAExtra");
  supportABackThick=Control.EvalVar<double>(keyName+"SupportABackThick");
  supportABackLength=Control.EvalVar<double>(keyName+"SupportABackLength");
  supportABase=Control.EvalVar<double>(keyName+"SupportABase");
  supportAPillar=Control.EvalVar<double>(keyName+"SupportAPillar");
  supportAPillarStep=Control.EvalVar<double>(keyName+"SupportAPillarStep");
 

  mirrorAMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorAMat");
  mirrorBMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorBMat");

  baseAMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseAMat");
  baseBMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseBMat");


  return;
}

void
MLMono::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("MLMono","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
MLMono::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("MLMono","createSurfaces");

  // main xstal CENTRE AT ORIGIN 
  const Geometry::Quaternion QXA
    (Geometry::Quaternion::calcQRotDeg(-thetaA,X));

  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Y);
  Geometry::Vec3D PZ(Z);

  QXA.rotate(PY);
  QXA.rotate(PZ);

  const Geometry::Quaternion QYA
    (Geometry::Quaternion::calcQRotDeg(phiA,PY));

  QYA.rotate(PX);
  QYA.rotate(PZ);
  
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-PY*(lengthA/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+PY*(lengthA/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-PX*widthA,PX);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin,PX);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-PZ*(heightA/2.0),PZ);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+PZ*(heightA/2.0),PZ);
  
  // support A:
  ModelSupport::buildPlane
    (SMap,buildIndex+201,Origin-PY*((lengthA+supportAExtra)/2.0),PY);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,Origin+PY*((lengthA+supportAExtra)/2.0),PY);
  ModelSupport::buildPlane
    (SMap,buildIndex+203,Origin-PX*(widthA+supportAGap),PX);  
  ModelSupport::buildPlane
    (SMap,buildIndex+205,Origin-PZ*(heightA/2+supportABase),PZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+206,Origin+PZ*(heightA/2+supportABase),PZ);

  ModelSupport::buildPlane
    (SMap,buildIndex+213,Origin-PX*(widthA+supportAGap+supportABackThick),PX);
  ModelSupport::buildPlane
    (SMap,buildIndex+211,Origin-PY*(supportABackLength/2.0),PY);
  ModelSupport::buildPlane
    (SMap,buildIndex+212,Origin+PY*(supportABackLength/2.0),PY);

  // note extra /2.0 as in middle of extra step!
  const Geometry::Vec3D PillarA=
    Origin-PY*((lengthA+supportAExtra/2.0)/2.0)-PX*supportAPillarStep;
  const Geometry::Vec3D PillarB=
    Origin-PY*((lengthA+supportAExtra/2.0)/2.0)-PX*(widthA-supportAPillarStep);
  const Geometry::Vec3D PillarC=
    Origin+PY*((lengthA+supportAExtra/2.0)/2.0)-PX*supportAPillarStep;
  const Geometry::Vec3D PillarD=
    Origin+PY*((lengthA+supportAExtra/2.0)/2.0)-PX*(widthA-supportAPillarStep);

  ModelSupport::buildCylinder
    (SMap,buildIndex+307,PillarA,PZ,supportAPillar);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,PillarB,PZ,supportAPillar);
  ModelSupport::buildCylinder
    (SMap,buildIndex+327,PillarC,PZ,supportAPillar);
  ModelSupport::buildCylinder
    (SMap,buildIndex+337,PillarD,PZ,supportAPillar);

  return; 
}

void
MLMono::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MLMono","createObjects");

  std::string Out;
  // xstal
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 103 -104 105 -106 ");  
  makeCell("XStalA",System,cellIndex++,mirrorAMat,0.0,Out);

  // Substrate
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 203 -104  106 -206 ");  
  makeCell("TopA",System,cellIndex++,baseAMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 203 -104  205 -105 ");  
  makeCell("BaseA",System,cellIndex++,baseAMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 203 -103 105 -106 ");  
  makeCell("GapA",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 211 -212 -203 213 205 -206 ");  
  makeCell("BackA",System,cellIndex++,baseAMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -211 -203 213 205 -206 ");  
  makeCell("BackAVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 212 -202 -203 213 205 -206 ");  
  makeCell("BackAVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -307 105 -106 ");
  makeCell("RodA1",System,cellIndex++,baseAMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -317 105 -106 ");
  makeCell("RodA2",System,cellIndex++,baseAMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -101 103 -104 105 -106 307 317 ");  
  makeCell("SideAVoid",System,cellIndex++,0,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," -327 105 -106 ");
  makeCell("RodA3",System,cellIndex++,baseAMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -337 105 -106 ");
  makeCell("RodA4",System,cellIndex++,baseAMat,0.0,Out);  
  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -202 103 -104 105 -106 327 337 ");  
  makeCell("SideAVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 213 -104 205 -206");  
  addOuterSurf(Out);
  
  return; 
}

void
MLMono::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MLMono","createLinks");
  
  return;
}

void
MLMono::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MLMono","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
