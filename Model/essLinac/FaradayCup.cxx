/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/FaradayCup.cxx
 *
 * Copyright (c) 2004-2022 by Konstantin Batkov
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
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "ContainedComp.h"
#include "FixedRotate.h"

#include "FaradayCup.h"

namespace essSystem
{

FaradayCup::FaradayCup(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FaradayCup::FaradayCup(const FaradayCup& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  active(A.active),
  engActive(A.engActive),
  length(A.length),outerRadius(A.outerRadius),innerRadius(A.innerRadius),
  faceLength(A.faceLength),
  faceRadius(A.faceRadius),
  absLength(A.absLength),
  absMat(A.absMat),
  baseLength(A.baseLength),
  colLength(A.colLength),
  colMat(A.colMat),wallMat(A.wallMat),
  airMat(A.airMat),
  shieldRadius(A.shieldRadius),
  shieldInnerRadius(A.shieldInnerRadius),
  shieldLength(A.shieldLength),
  shieldInnerLength(A.shieldInnerLength),
  shieldMat(A.shieldMat)
  /*!
    Copy constructor
    \param A :: FaradayCup to copy
  */
{}

FaradayCup&
FaradayCup::operator=(const FaradayCup& A)
  /*!
    Assignment operator
    \param A :: FaradayCup to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      active=A.active;
      engActive=A.engActive;
      length=A.length;
      outerRadius=A.outerRadius;
      innerRadius=A.innerRadius;
      faceLength=A.faceLength;
      faceRadius=A.faceRadius;
      absLength=A.absLength;
      absMat=A.absMat;
      baseLength=A.baseLength;
      colLength=A.colLength;
      colMat=A.colMat;
      wallMat=A.wallMat;
      airMat=A.airMat;
      shieldRadius=A.shieldRadius;
      shieldInnerRadius=A.shieldInnerRadius;
      shieldLength=A.shieldLength;
      shieldInnerLength=A.shieldInnerLength;
      shieldMat=A.shieldMat;
    }
  return *this;
}

FaradayCup*
FaradayCup::clone() const
  /*!
    Clone self
    \return new (this)
  */
{
  return new FaradayCup(*this);
}

FaradayCup::~FaradayCup()
  /*!
    Destructor
  */
{}

void
FaradayCup::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FaradayCup","populate");

  FixedRotate::populate(Control);
  active=Control.EvalDefVar<int>(keyName+"Active", 1);
  engActive=Control.EvalTail<int>(keyName,"","EngineeringActive");

  length=Control.EvalVar<double>(keyName+"Length");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  faceLength=Control.EvalVar<double>(keyName+"FaceLength");
  faceRadius=Control.EvalVar<double>(keyName+"FaceRadius");
  absLength=Control.EvalVar<double>(keyName+"AbsorberLength");
  absMat=ModelSupport::EvalMat<int>(Control,keyName+"AbsorberMat");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");
  colLength=Control.EvalVar<double>(keyName+"CollectorLength");
  colMat=ModelSupport::EvalMat<int>(Control,keyName+"CollectorMat");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");

  shieldRadius=Control.EvalVar<double>(keyName+"ShieldRadius");
  shieldInnerRadius=Control.EvalVar<double>(keyName+"ShieldInnerRadius");
  shieldLength=Control.EvalVar<double>(keyName+"ShieldLength");
  shieldInnerLength=Control.EvalVar<double>(keyName+"ShieldInnerLength");
  shieldMat=ModelSupport::EvalMat<int>(Control,keyName+"ShieldMat");

  return;
}

void
FaradayCup::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FaradayCup","createSurfaces");

  double dy(0.0);
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*dy,Y);
  dy += faceLength;
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*dy,Y);
  dy += absLength;
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*dy,Y);
  dy += baseLength;
  ModelSupport::buildPlane(SMap,buildIndex+31,Origin+Y*dy,Y);
  dy += colLength;
  ModelSupport::buildPlane(SMap,buildIndex+41,Origin+Y*dy,Y);

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,outerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,faceRadius);

  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(shieldInnerLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+112,Origin+Y*(shieldLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,shieldInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,Y,shieldRadius);

  return;
}

void
FaradayCup::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FaradayCup","createObjects");

  if (active)
    {
      HeadRule HR;
      // collimator
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 -27");
      System.addCell(cellIndex++,airMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 27 -17");
      System.addCell(cellIndex++,wallMat,0.0,HR);
      
      // absorber
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -17");
      System.addCell(cellIndex++,absMat,0.0,HR);
      
      // base
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 -7");
      System.addCell(cellIndex++,0,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -41 7 -17");
      System.addCell(cellIndex++,wallMat,0.0,HR);
      
      // collector
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"31 -41 -7");
      System.addCell(cellIndex++,colMat,0.0,HR);
      
      // back plane
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"41 -2 -17");
      System.addCell(cellIndex++,wallMat,0.0,HR);
      
      // shielding
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -102 -107 (-1:2:17)");
      System.addCell(cellIndex++,airMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -112 -117 (-1:102:107)");
      System.addCell(cellIndex++,shieldMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -112 -117");
      addOuterSurf(HR);
    }
  return;
}


void
FaradayCup::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("FaradayCup","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  return;
}




void
FaradayCup::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FaradayCup","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
