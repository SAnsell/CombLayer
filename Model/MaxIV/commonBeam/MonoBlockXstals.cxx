/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoBlockXstals.cxx
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

#include "MonoBlockXstals.h"


namespace xraySystem
{

MonoBlockXstals::MonoBlockXstals(const std::string& Key) :
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


MonoBlockXstals::~MonoBlockXstals()
  /*!
    Destructor
   */
{}

void
MonoBlockXstals::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MonoBlockXstals","populate");

  FixedRotate::populate(Control);

  gap=Control.EvalVar<double>(keyName+"Gap");
  theta=Control.EvalVar<double>(keyName+"Theta");
  phiA=Control.EvalDefVar<double>(keyName+"PhiA",0.0);
  phiB=Control.EvalDefVar<double>(keyName+"PhiB",0.0);
  
  widthA=Control.EvalPair<double>(keyName+"WidthA",keyName+"Width");
  heightA=Control.EvalPair<double>(keyName+"HeightA",keyName+"Height");
  lengthA=Control.EvalPair<double>(keyName+"LengthA",keyName+"Length");

  widthB=Control.EvalPair<double>(keyName+"WidthB",keyName+"Width");
  heightB=Control.EvalPair<double>(keyName+"HeightB",keyName+"Height");
  lengthB=Control.EvalPair<double>(keyName+"LengthB",keyName+"Length");
  
  baseALength=Control.EvalPair<double>(keyName+"BaseALength",keyName+"BaseLength");
  baseAHeight=Control.EvalPair<double>(keyName+"BaseAHeight",keyName+"BaseHeight");
  baseAWidth=Control.EvalPair<double>(keyName+"BaseAWidth",keyName+"BaseWidth");

  topALength=Control.EvalPair<double>(keyName+"TopALength",keyName+"TopLength");
  topAHeight=Control.EvalPair<double>(keyName+"TopAHeight",keyName+"TopHeight");
  topAWidth=Control.EvalPair<double>(keyName+"TopAWidth",keyName+"TopWidth");

  baseBLength=Control.EvalPair<double>(keyName+"BaseBLength",keyName+"BaseLength");
  baseBHeight=Control.EvalPair<double>(keyName+"BaseBHeight",keyName+"BaseHeight");
  baseBWidth=Control.EvalPair<double>(keyName+"BaseBWidth",keyName+"BaseWidth");

  topBLength=Control.EvalPair<double>(keyName+"TopBLength",keyName+"TopLength");
  topBHeight=Control.EvalPair<double>(keyName+"TopBHeight",keyName+"TopHeight");
  topBWidth=Control.EvalPair<double>(keyName+"TopBWidth",keyName+"TopWidth");

  xtalMat=ModelSupport::EvalMat<int>(Control,keyName+"XtalMat");
  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");

  return;
}

void
MonoBlockXstals::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("MonoBlockXstals","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
MonoBlockXstals::createSurfaces()
  /*!
    Create planes for the silicon and supports
  */
{
  ELog::RegMethod RegA("MonoBlockXstals","createSurfaces");

  // main xstal CENTRE AT ORIGIN 
  const Geometry::Quaternion QXTheta
    (Geometry::Quaternion::calcQRotDeg(theta,Z));
  const Geometry::Quaternion QXA
    (Geometry::Quaternion::calcQRotDeg(phiA,Y));
  const Geometry::Quaternion QXB
    (Geometry::Quaternion::calcQRotDeg(phiB,Y));

  Geometry::Vec3D AX(X);
  Geometry::Vec3D AY(Y);
  Geometry::Vec3D AZ(Z);

  QXTheta.rotate(AX);
  QXTheta.rotate(AY);

  Geometry::Vec3D BX(AX);  // same rotation
  Geometry::Vec3D BY(AY);  // same rotation
  Geometry::Vec3D BZ(Z);

  QXA.rotate(AX);
  QXA.rotate(AY);
  QXA.rotate(AZ);
  
  QXB.rotate(BX);
  QXB.rotate(BY);
  QXB.rotate(BZ);

  // crystal 1 (right hand side: AX points away from centre):
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-AY*(lengthA/2.0),AY);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+AY*(lengthA/2.0),AY);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin,AX);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+AX*widthA,AX);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-AZ*(heightA/2.0),AZ);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+AZ*(heightA/2.0),AZ);

  // crystal 2: (left hand side BX points to centre)
  const Geometry::Vec3D bOrg=
    Origin+Y*(gap/tan(theta*2.0*M_PI/180.0))-X*gap;
  ModelSupport::buildPlane(SMap,buildIndex+201,bOrg-BY*(lengthB/2.0),BY);
  ModelSupport::buildPlane(SMap,buildIndex+202,bOrg+BY*(lengthB/2.0),BY);
  ModelSupport::buildPlane(SMap,buildIndex+203,bOrg-BX*widthB,BX);
  ModelSupport::buildPlane(SMap,buildIndex+204,bOrg,BX);
  ModelSupport::buildPlane(SMap,buildIndex+205,bOrg-BZ*(heightB/2.0),BZ);
  ModelSupport::buildPlane(SMap,buildIndex+206,bOrg+BZ*(heightB/2.0),BZ);

  // Top Crystal [base block +Z]
  ModelSupport::buildPlane(SMap,buildIndex+1001,Origin-AY*(baseALength/2.0),AY);
  ModelSupport::buildPlane(SMap,buildIndex+1002,Origin+AY*(baseALength/2.0),AY);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+AX*baseAWidth,AX);
  ModelSupport::buildPlane(SMap,buildIndex+1005,Origin-AZ*(baseAHeight+heightA/2.0),AZ);

  // Top Crystal [top block +Z]
  ModelSupport::buildPlane(SMap,buildIndex+1101,Origin-AY*(topALength/2.0),AY);
  ModelSupport::buildPlane(SMap,buildIndex+1102,Origin+AY*(topALength/2.0),AY);
  ModelSupport::buildPlane(SMap,buildIndex+1104,Origin+AX*topAWidth,AX);
  ModelSupport::buildPlane(SMap,buildIndex+1106,Origin+AZ*(topAHeight+heightA/2.0),AZ);

  // Top Crystal [base block +Z]
  ModelSupport::buildPlane(SMap,buildIndex+2001,bOrg-BY*(baseBLength/2.0),BY);
  ModelSupport::buildPlane(SMap,buildIndex+2002,bOrg+BY*(baseBLength/2.0),BY);
  ModelSupport::buildPlane(SMap,buildIndex+2003,bOrg-BX*baseBWidth,BX);
  ModelSupport::buildPlane(SMap,buildIndex+2005,bOrg-BZ*(baseBHeight+heightB/2.0),BZ);

  // Top Crystal [top block +Z]
  ModelSupport::buildPlane(SMap,buildIndex+2101,bOrg-BY*(topBLength/2.0),BY);
  ModelSupport::buildPlane(SMap,buildIndex+2102,bOrg+BY*(topBLength/2.0),BY);
  ModelSupport::buildPlane(SMap,buildIndex+2103,bOrg-BX*topBWidth,BX);
  ModelSupport::buildPlane(SMap,buildIndex+2106,bOrg+BZ*(topBHeight+heightB/2.0),BZ);

  // Quick spacing block for base/lower crystals:
  const double ALMax(std::max(topALength,baseALength));
  const double AWMax(std::max(topAWidth,baseAWidth));
  const double BLMax(std::max(topBLength,baseBLength));
  const double BWMax(std::max(topBWidth,baseBWidth));

  ModelSupport::buildPlane(SMap,buildIndex+3001,Origin-AY*(ALMax/2.0),AY);
  ModelSupport::buildPlane(SMap,buildIndex+3002,Origin+AY*(ALMax/2.0),AY);
  ModelSupport::buildPlane(SMap,buildIndex+3004,Origin+AX*AWMax,AX);

  ModelSupport::buildPlane(SMap,buildIndex+4001,bOrg-BY*(BLMax/2.0),BY);
  ModelSupport::buildPlane(SMap,buildIndex+4002,bOrg+BY*(BLMax/2.0),BY);
  ModelSupport::buildPlane(SMap,buildIndex+4003,bOrg-BX*BWMax,BX);


  return; 
}

void
MonoBlockXstals::createObjects(Simulation& System)
  /*!
    Create the object for the MonoBlock
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MonoBlockXstals","createObjects");

  std::string Out;
  // xstal A
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("XtalA",System,cellIndex++,xtalMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 3001 -3002 103 -3004 105 -106 (-101:102:104)");
  makeCell("XtalAVoid",System,cellIndex++,0,0.0,Out);
  System.minimizeObject(cellIndex-1);

  // base block
  Out=ModelSupport::getComposite(SMap,buildIndex," 1001 -1002 103 -1004 1005 -105 ");
  makeCell("BaseA",System,cellIndex++,baseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 3001 -3002 103 -3004 1005 -105 (-1001:1002:1004)");
  makeCell("BaseAVoid",System,cellIndex++,0,0.0,Out);
  System.minimizeObject(cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1101 -1102 103 -1104 -1106 106 ");
  makeCell("TopA",System,cellIndex++,baseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 3001 -3002 103 -3004 -1106 106 (-1101:1102:1104)");
  makeCell("TopAVoid",System,cellIndex++,0,0.0,Out);
  System.minimizeObject(cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3001 -3002 103 -3004 -1106 1005 ");
  addOuterUnionSurf(Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -202 203 -204 205 -206 ");
  makeCell("XtalB",System,cellIndex++,xtalMat,0.0,Out);  
  Out=ModelSupport::getComposite(SMap,buildIndex," 4001 -4002 -204 4003 205 -206 (-201:202:-203)");
  makeCell("XtalBVoid",System,cellIndex++,0,0.0,Out);
  System.minimizeObject(cellIndex-1);
  
  // base block
  Out=ModelSupport::getComposite(SMap,buildIndex," 2001 -2002 -204 2003 2005 -205 ");
  makeCell("BaseB",System,cellIndex++,baseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 4001 -4002 -204 4003 2005 -205 (-2001:2002:-2003)");
  makeCell("BaseBVoid",System,cellIndex++,0,0.0,Out);
  System.minimizeObject(cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2101 -2102 -204 2103 -2106 206 ");
  makeCell("TopB",System,cellIndex++,baseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 4001 -4002 -204 4003 -2106 206 (-2101:2102:-2103)");
  makeCell("TopBVoid",System,cellIndex++,0,0.0,Out);
  System.minimizeObject(cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex," 4001 -4002 -204 4003 -2106 2005 ");
  addOuterUnionSurf(Out);

  return; 
}

void
MonoBlockXstals::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MonoBlockXstals","createLinks");

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
MonoBlockXstals::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MonoBlockXstals","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
