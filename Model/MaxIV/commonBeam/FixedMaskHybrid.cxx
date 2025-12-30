/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/FixedMaskHybrid.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "FixedMaskHybrid.h"

namespace xraySystem
{

FixedMaskHybrid::FixedMaskHybrid(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FixedMaskHybrid::FixedMaskHybrid(const FixedMaskHybrid& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),radius(A.radius),flangeLength(A.flangeLength),
  flangeRadius(A.flangeRadius),
  outWidth(A.outWidth),
  outHeight(A.outHeight),
  outAngle(A.outAngle),
  outStraightLength(A.outStraightLength),
  mat(A.mat),flangeMat(A.flangeMat),
  voidMat(A.voidMat),
  airMat(A.airMat)
  /*!
    Copy constructor
    \param A :: FixedMaskHybrid to copy
  */
{}

FixedMaskHybrid&
FixedMaskHybrid::operator=(const FixedMaskHybrid& A)
  /*!
    Assignment operator
    \param A :: FixedMaskHybrid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      radius=A.radius;
      flangeLength=A.flangeLength;
      flangeRadius=A.flangeRadius;
      outWidth=A.outWidth;
      outHeight=A.outHeight;
      outAngle=A.outAngle;
      outStraightLength=A.outStraightLength;
      mat=A.mat;
      flangeMat=A.flangeMat;
      voidMat=A.voidMat;
      airMat=A.airMat;
    }
  return *this;
}

FixedMaskHybrid*
FixedMaskHybrid::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new FixedMaskHybrid(*this);
}

FixedMaskHybrid::~FixedMaskHybrid()
  /*!
    Destructor
  */
{}

void
FixedMaskHybrid::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  if (flangeLength*2 + Geometry::zeroTol > length)
    throw ColErr::IndexError<double>(flangeLength, length/2.0, keyName+"FlangeLength");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  outHeight=Control.EvalVar<double>(keyName+"OutHeight");
  outAngle=Control.EvalVar<double>(keyName+"OutAngle");
  outStraightLength=Control.EvalVar<double>(keyName+"OutStraightLength");
  if (flangeRadius < radius + Geometry::zeroTol)
    throw ColErr::SizeError<double>(flangeRadius, radius, keyName+"FlangeRadius");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat", "Void");
  airMat =ModelSupport::EvalDefMat(Control,keyName+"AirMat",  "Void");

  return;
}

void
FixedMaskHybrid::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","createSurfaces");

  if (!frontActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
  }
  if (!backActive()) {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
  }

  makeShiftedSurf(SMap, "front", buildIndex+11, Y, flangeLength);
  makeShiftedSurf(SMap, "back", buildIndex+12, Y, -flangeLength);
  makeShiftedSurf(SMap, "back", buildIndex+22, Y, -outStraightLength);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,flangeRadius);

  // Exit aperture - tilted segment
  const Geometry::Vec3D dY(Y*(length-outStraightLength));
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+33,Origin-X*(outWidth/2.0)+dY,X,Z,-outAngle/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+34,Origin+X*(outWidth/2.0)+dY,X,Z,outAngle/2.0);

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+35,Origin-Z*(outHeight/2.0)+dY,Z,X,outAngle/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+36,Origin+Z*(outHeight/2.0)+dY,Z,X,-outAngle/2.0);

  // Exit aperture - straight segment
  ModelSupport::buildPlane(SMap,buildIndex+43,Origin-X*(outWidth/2.0), X);
  ModelSupport::buildPlane(SMap,buildIndex+44,Origin+X*(outWidth/2.0), X);

  ModelSupport::buildPlane(SMap,buildIndex+45,Origin-Z*(outHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,Origin+Z*(outHeight/2.0),Z);


  return;
}

void
FixedMaskHybrid::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","createObjects");

  HeadRule HR;
  const HeadRule front(frontRule());
  const HeadRule back(backRule());

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -22 (-33:34:-35:36) -7 ");
  makeCell("MainCell",System,cellIndex++,mat,0.0,HR*front);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 22 (-43:44:-45:46) -7 ");
  makeCell("MainCell",System,cellIndex++,mat,0.0,HR*back);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -22 33 -34 35 -36 ");
  makeCell("ExitTilted",System,cellIndex++,voidMat,0.0,HR*front);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 22 43 -44 45 -46 ");
  makeCell("ExitStraight",System,cellIndex++,voidMat,0.0,HR*back);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -11 7 -17 ");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,HR*front);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 11 -12 7 -17 ");
  makeCell("OuterVoid",System,cellIndex++,airMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 12 7 -17 ");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,HR*back);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -17 ");
  addOuterSurf(HR*front*back);

  return;
}


void
FixedMaskHybrid::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  return;
}

void
FixedMaskHybrid::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FixedMaskHybrid","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
