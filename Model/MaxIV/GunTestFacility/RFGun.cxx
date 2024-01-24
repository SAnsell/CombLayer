/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/RFGun.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "General.h"
#include "Plane.h"


#include "RFGun.h"

namespace xraySystem
{

RFGun::RFGun(const std::string& Key)  :
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

RFGun::RFGun(const RFGun& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),cavityRadius(A.cavityRadius),cavityLength(A.cavityLength),
  cavityOffset(A.cavityOffset),
  irisThick(A.irisThick),
  irisRadius(A.irisRadius),
  irisStretch(A.irisStretch),
  wallThick(A.wallThick),
  nFrameFacets(A.nFrameFacets),
  frameWidth(A.frameWidth),
  frontPreFlangeThick(A.frontPreFlangeThick),
  frontPreFlangeRadius(A.frontPreFlangeRadius),
  frontFlangeThick(A.frontFlangeThick),
  frontFlangeRadius(A.frontFlangeRadius),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: RFGun to copy
  */
{}

RFGun&
RFGun::operator=(const RFGun& A)
  /*!
    Assignment operator
    \param A :: RFGun to copy
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
      cavityRadius=A.cavityRadius;
      cavityLength=A.cavityLength;
      irisThick=A.irisThick;
      cavityOffset=A.cavityOffset;
      irisRadius=A.irisRadius;
      irisStretch=A.irisStretch;
      wallThick=A.wallThick;
      nFrameFacets=A.nFrameFacets;
      frameWidth=A.frameWidth;
      frontPreFlangeThick=A.frontPreFlangeThick;
      frontPreFlangeRadius=A.frontPreFlangeRadius;
      frontFlangeThick=A.frontFlangeThick;
      frontFlangeRadius=A.frontFlangeRadius;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

RFGun*
RFGun::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new RFGun(*this);
}

RFGun::~RFGun()
  /*!
    Destructor
  */
{}

std::string
RFGun::irisSurf(const double A, const double R, const double y)
/*!
  Return sq surface with two-sheeted hyperboloid of revolution (iris surface)
 */
{
  ELog::RegMethod RegA("RFGun","irisSurf");

  const double B = -1;
  const double C = A;
  const double G = -C*R*R;
  std::ostringstream cx;
  cx<<"sq " << A << " " << B << " " << C << " 0 0 0 " << G << " 0 " << y << " 0";
  return cx.str();
}

double
RFGun::getFrameRadius() const
/*!
  Return the radius of the circumscribed circle around the frame
 */
{
  ELog::RegMethod RegA("RFGun","getFrameRadius");

  const double n = static_cast<double>(nFrameFacets);
  const double angle = M_PI/n; // half of angle between the facets
  const double a = frameWidth*tan(angle); // facet length (2D)

  return a/(2.0 * sin(M_PI/n));
}


void
RFGun::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("RFGun","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  cavityRadius=Control.EvalVar<double>(keyName+"CavityRadius");
  cavityLength=Control.EvalVar<double>(keyName+"CavityLength");
  cavityOffset=Control.EvalVar<double>(keyName+"CavityOffset");
  irisThick=Control.EvalVar<double>(keyName+"IrisThick");
  irisRadius=Control.EvalVar<double>(keyName+"IrisRadius");
  irisStretch=Control.EvalVar<double>(keyName+"IrisStretch");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  nFrameFacets=Control.EvalVar<size_t>(keyName+"NFrameFacets");
  frameWidth=Control.EvalVar<double>(keyName+"FrameWidth");
  frontPreFlangeThick=Control.EvalVar<double>(keyName+"FrontPreFlangeThick");
  frontPreFlangeRadius=Control.EvalVar<double>(keyName+"FrontPreFlangeRadius");
  frontFlangeThick=Control.EvalVar<double>(keyName+"FrontFlangeThick");
  frontFlangeRadius=Control.EvalVar<double>(keyName+"FrontFlangeRadius");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
RFGun::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RFGun","createSurfaces");

   if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(frontPreFlangeThick),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
              SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
                                      frontPreFlangeThick);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length+wallThick),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+12));

      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
              SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
                                      -wallThick);
    }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,frontPreFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,frontFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,cavityRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Y,irisRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+47,Origin,Y,irisRadius+wallThick);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+21,buildIndex+1,Y,frontFlangeThick);
  const auto p22 = ModelSupport::buildShiftedPlane(SMap,buildIndex+22,buildIndex+21,Y,cavityOffset);
  const auto p31 = ModelSupport::buildShiftedPlane(SMap,buildIndex+31,buildIndex+22,Y,irisThick);
  const auto p32 = ModelSupport::buildShiftedPlane(SMap,buildIndex+32,buildIndex+31,Y,cavityLength);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+42,buildIndex+32,Y,irisThick/2.0);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+43,buildIndex+32,Y,wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+53,buildIndex+42,Y,wallThick); // dummy

  int CN(buildIndex+1000);
  double angle(0.0);
  const double dangle = 2.0*M_PI/static_cast<double>(nFrameFacets);
  for (size_t i=0; i<nFrameFacets; ++i) {
    const Geometry::Vec3D QR=X*cos(angle)+Z*sin(angle);
    ModelSupport::buildPlane(SMap,CN+51,Origin+QR*(frameWidth/2.0),QR);
    angle+=dangle;
    CN++;
  }
  ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,getFrameRadius()+1.0);

  // Iris1 surface
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  double y = (p22->getDistance() + p31->getDistance())/2.0;
  Geometry::General *GA = SurI.createUniqSurf<Geometry::General>(buildIndex+517);
  GA->setSurface(irisSurf(irisStretch, irisRadius, y));
  SMap.registerSurf(GA);

  // Iris2 surface
  y += cavityLength+irisThick;
  GA = SurI.createUniqSurf<Geometry::General>(buildIndex+527);
  GA->setSurface(irisSurf(irisStretch, irisRadius, y));
  SMap.registerSurf(GA);

  // Back surface
  GA = SurI.createUniqSurf<Geometry::General>(buildIndex+537);
  GA->setSurface(irisSurf(irisStretch, irisRadius+wallThick, y+wallThick));
  SMap.registerSurf(GA);



  // ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,cavityRadius+wallThick);

  // const auto p21 = ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*(cavityOffset+frontPreFlangeThick+frontFlangeThick),Y);
  // ModelSupport::buildShiftedPlane(SMap,buildIndex+22,buildIndex+21,Y,cavityLength);
  // const auto p31 = ModelSupport::buildShiftedPlane(SMap,buildIndex+31,buildIndex+21,Y,-irisThick);
  // ModelSupport::buildShiftedPlane(SMap,buildIndex+32,buildIndex+22,Y,irisThick);


  return;
}

void
RFGun::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RFGun","createObjects");

  HeadRule HR;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -31 -7 ");
  // makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 32 -2 -7 ");
  // makeCell("MainCell",System,cellIndex++,mainMat,0.0,HR);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 7 -17 ");
  // makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -1 -7 ");
  makeCell("FrontFlangePre",System,cellIndex++,wallMat,0.0,HR*frontStr);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -1 7 -1007");
  makeCell("FrontFlangePreVoid",System,cellIndex++,0,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -21 -17 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,HR*frontStr);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -21 17 -1007 ");
  makeCell("FrontFlangeVoid",System,cellIndex++,0,0.0,HR*frontStr);

  const std::string strFrame=
    ModelSupport::getSeqIntersection(-51, -(50+static_cast<int>(nFrameFacets)),1);
  const HeadRule HRFrame=ModelSupport::getHeadRule(SMap,buildIndex+1000,strFrame);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"21 27 -43");
  HR *= HRFrame;
  makeCell("Frame",System,cellIndex++,wallMat,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"21 -1007 -43");
  HR *= HRFrame.complement();
  makeCell("FrameOuterVoid",System,cellIndex++,0,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"27 -1007 43");
  makeCell("FrameBackVoid",System,cellIndex++,0,0.0,HR*backStr);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," 21 -22 -27 ");
  makeCell("06Cavity",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 22 -31 -27 517 ");
  makeCell("IrisWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 22 -31 -517 ");
  makeCell("Iris",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 -27");
  makeCell("Cavity",System,cellIndex++,0,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," 32 -42 -27 527 ");
  makeCell("BackWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 42 -37 : (32 -42 -527)");
  makeCell("BackFlangeVoid",System,cellIndex++,0,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 42 37 -53 -537");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"53 37 -47");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"42 -53 -27 537 : 53 47 -27");
  makeCell("BackFlangeOuterVoid",System,cellIndex++,0,0.0,HR*backStr);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," -1 27 -17");
  // makeCell("FrontFlange",System,cellIndex++,0,0.0,HR*frontStr);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 2 -17 ");
  // makeCell("WallBack",System,cellIndex++,wallMat,0.0,HR*backStr);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 21 -22 -7 ");
  // makeCell("MainCavity",System,cellIndex++,0,0.0,HR);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 31 -21 -7 517 ");
  // makeCell("MainCavityWallFront",System,cellIndex++,wallMat,0.0,HR);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 31 -21  -517 ");
  // makeCell("MainCavityIrisFront",System,cellIndex++,mainMat,0.0,HR);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex," 22 -32 -7 ");
  // makeCell("MainCavityWallBack",System,cellIndex++,wallMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," -1007");
  addOuterSurf(HR*frontStr*backStr);

  return;
}


void
RFGun::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("RFGun","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(cavityRadius/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(cavityRadius/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(cavityLength/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(cavityLength/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
RFGun::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("RFGun","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem