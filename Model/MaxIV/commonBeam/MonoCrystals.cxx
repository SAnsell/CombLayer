/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoCrystals.cxx
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
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"

#include "MonoCrystals.h"


namespace xraySystem
{

MonoCrystals::MonoCrystals(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,8),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  nameSideIndex(0,"beamIn");
  nameSideIndex(1,"beamOut");
}


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

  FixedRotate::populate(Control);

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
  
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-PY*(lengthA/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+PY*(lengthA/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-PX*(widthA/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+PX*(widthA/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-PZ*thickA,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin,PZ);

  // support:
  ModelSupport::buildPlane(SMap,buildIndex+1001,Origin-PY*(baseLength/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+1002,Origin+PY*(baseLength/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+1003,Origin-PX*(baseExtra+widthA/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+PX*(baseExtra+widthA/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+1005,Origin-PZ*(thickA+baseGap+baseThick),PZ);
  ModelSupport::buildPlane(SMap,buildIndex+1006,Origin-PZ*(thickA+baseGap),PZ);


  const Geometry::Vec3D BOrg=
    Origin+Y*(gap/tan(theta*2.0*M_PI/180.0))+Z*gap;
  
  ModelSupport::buildPlane(SMap,buildIndex+201,BOrg-PY*(lengthB/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+202,BOrg+PY*(lengthB/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+203,BOrg-PX*(widthB/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+204,BOrg+PX*(widthB/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+205,BOrg,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+206,BOrg+PZ*thickB,PZ);

  
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
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("XtalA",System,cellIndex++,xtalMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -1002 1003 -1004 1005 -106 (-1006:-103:104)");
  makeCell("ABase",System,cellIndex++,baseMat,0.0,Out);

  std::string frontBack;
  if (baseLength-lengthA > Geometry::zeroTol)
    {
      // extra parts of void crystal:
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 1001 -101  103 -104 105 -106 ");
      makeCell("AFrontVoid",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex," 102 -1002 103 -104 105 -106  ");
      makeCell("ABackVoid",System,cellIndex++,0,0.0,Out);

      frontBack=ModelSupport::getComposite(SMap,buildIndex," 1001 -1002 ");
    }
  else if (baseLength-lengthA < -Geometry::zeroTol)
    {
      // extra void parts of base crystal:
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 101 -1001  1003 -1004 -106 1005 (-1006:-103:104)");
      makeCell("AFrontVoid",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex," -102 1002  1003 -1004 -106 1005 (-1006:-103:104)");
      makeCell("ABackVoid",System,cellIndex++,0,0.0,Out);
      frontBack=ModelSupport::getComposite(SMap,buildIndex," 101 -102 ");
    }
  
  // air gap:
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 103 -104 -106 1005 ");
  makeCell("ABaseVoid",System,cellIndex++,0,0.0,Out+frontBack);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1003 -1004 -106 1005 ");
  addOuterSurf(Out+frontBack);


  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 203 -204 205 -206 ");
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

  const Geometry::Vec3D BOrg=
    Origin+Y*(gap/tan(theta*2.0*M_PI/180.0))+Z*gap;

  // top surface going back down beamline to ring
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+106));

  // top surface going to experimental area
  FixedComp::setConnect(1,BOrg,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+205));

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
