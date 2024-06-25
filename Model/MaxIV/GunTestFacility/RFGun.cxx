/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/RFGun.cxx
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
  attachSystem::FixedRotate(Key,7),
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
  length(A.length),
  frontTubeLength(A.frontTubeLength),
  frontTubeRadius(A.frontTubeRadius),
  frontTubeFlangeLength(A.frontTubeFlangeLength),
  frontTubeFlangeRadius(A.frontTubeFlangeRadius),
  frontTubePipeThick(A.frontTubePipeThick),
  backTubeRadius(A.backTubeRadius),
  backTubeFlangeLength(A.backTubeFlangeLength),
  backTubeFlangeRadius(A.backTubeFlangeRadius),
  backTubePipeThick(A.backTubePipeThick),
  cavityRadius(A.cavityRadius),cavityLength(A.cavityLength),
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
  insertWidth(A.insertWidth),
  insertLength(A.insertLength),
  insertDepth(A.insertDepth),
  insertWallThick(A.insertWallThick),
  insertCut(A.insertCut),
  mainMat(A.mainMat),wallMat(A.wallMat),
  frontTubePipeMat(A.frontTubePipeMat),
  backTubePipeMat(A.backTubePipeMat)
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
      frontTubeLength=A.frontTubeLength;
      frontTubeRadius=A.frontTubeRadius;
      frontTubeFlangeLength=A.frontTubeFlangeLength;
      frontTubeFlangeRadius=A.frontTubeFlangeRadius;
      frontTubePipeThick=A.frontTubePipeThick;
      backTubeRadius=A.backTubeRadius;
      backTubeFlangeLength=A.backTubeFlangeLength;
      backTubeFlangeRadius=A.backTubeFlangeRadius;
      backTubePipeThick=A.backTubePipeThick;
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
      insertWidth=A.insertWidth;
      insertLength=A.insertLength;
      insertDepth=A.insertDepth;
      insertWallThick=A.insertWallThick;
      insertCut=A.insertCut;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      frontTubePipeMat=A.frontTubePipeMat;
      backTubePipeMat=A.backTubePipeMat;
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
  cx<<"sq " << A << " " << B << " " << C << " 0 0 0 " << G << " " << Origin + Y*y;
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
  frontTubeLength=Control.EvalVar<double>(keyName+"FrontTubeLength");
  frontTubeRadius=Control.EvalVar<double>(keyName+"FrontTubeRadius");
  frontTubeFlangeLength=Control.EvalVar<double>(keyName+"FrontTubeFlangeLength");
  frontTubeFlangeRadius=Control.EvalVar<double>(keyName+"FrontTubeFlangeRadius");
  frontTubePipeThick=Control.EvalVar<double>(keyName+"FrontTubePipeThick");
  backTubeRadius=Control.EvalVar<double>(keyName+"BackTubeRadius");
  backTubeFlangeLength=Control.EvalVar<double>(keyName+"BackTubeFlangeLength");
  backTubeFlangeRadius=Control.EvalVar<double>(keyName+"BackTubeFlangeRadius");
  backTubePipeThick=Control.EvalVar<double>(keyName+"BackTubePipeThick");
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
  insertWidth=Control.EvalVar<double>(keyName+"InsertWidth");
  insertLength=Control.EvalVar<double>(keyName+"InsertLength");
  insertDepth=Control.EvalVar<double>(keyName+"InsertDepth");
  insertWallThick=Control.EvalVar<double>(keyName+"InsertWallThick");
  insertCut=Control.EvalVar<double>(keyName+"InsertCut");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  frontTubePipeMat=ModelSupport::EvalMat<int>(Control,keyName+"FrontTubePipeMat");
  backTubePipeMat=ModelSupport::EvalMat<int>(Control,keyName+"BackTubePipeMat");

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

      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin+Y*(frontTubeLength-frontPreFlangeThick-frontFlangeThick),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
              SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
                                      frontTubeLength-frontPreFlangeThick-frontFlangeThick);
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
  double y = Origin.Y() + (p22->getDistance() + p31->getDistance())/2.0;
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

  // insert
  y = Origin.Y() + (p31->getDistance() + p32->getDistance())/2.0;
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(y-insertLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(y+insertLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(insertWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(insertWidth/2.0),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+105,buildIndex+1057,Z,insertDepth);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+106,buildIndex+1053,Z,-insertDepth);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+111,buildIndex+101,Y,-insertWallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+112,buildIndex+102,Y,insertWallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+113,buildIndex+103,X,-insertWallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+114,buildIndex+104,X,insertWallThick);

  // insert cut
  ModelSupport::buildPlane(SMap,buildIndex+123,Origin-X*(insertCut/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+124,Origin+X*(insertCut/2.0),X);

  // back tube
  ModelSupport::buildShiftedPlane(SMap,buildIndex+201,buildIndex+1,Y,-frontPreFlangeThick);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*frontTubeFlangeLength,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,frontTubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,Origin,Y,frontTubeRadius+frontTubePipeThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,Origin,Y,frontTubeFlangeRadius);

  // front tube
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,backTubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin,Y,backTubeRadius+backTubePipeThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin,Y,backTubeFlangeRadius);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+301,
              SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				  -backTubeFlangeLength);



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

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -201 -7 ");
  makeCell("preVoid",System,cellIndex++,0,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 201 -1 -7 ");
  makeCell("FrontFlangePre",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -1 7 -1007 -207");
  makeCell("FrontFlangePreVoid",System,cellIndex++,0,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -202 227 -1007");
  makeCell("VoidAboveFlange",System,cellIndex++,0,0.0,HR*frontStr);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 202 -21 217 -1007");
  makeCell("FrontFlangePreVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -21 -17 ");
  makeCell("Cathode",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -21 207 -217 ");
  makeCell("BackTube",System,cellIndex++,frontTubePipeMat,0.0,HR*frontStr);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -202 217 -227 ");
  makeCell("BackTubeFlange",System,cellIndex++,frontTubePipeMat,0.0,HR*frontStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -21 17 -207 ");
  makeCell("FrontFlangeVoid",System,cellIndex++,0,0.0,HR);

  // insert
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"27 101 -102 103 -104 105 -106 -123");
  makeCell("InsertBase",System,cellIndex++,wallMat,0.0,HR);
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"27 101 -102 103 -104 105 -106 124");
  makeCell("InsertBase",System,cellIndex++,wallMat,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"27 101 -102 123 -124 105 -106");
  makeCell("InsertCut",System,cellIndex++,0,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 -105 -1057 ");
  makeCell("InsertLower",System,cellIndex++,0,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 106 -1053 ");
  makeCell("InsertUpper",System,cellIndex++,0,0.0,HR);

  // Insert wall
  HR = ModelSupport::getHeadRule(SMap,buildIndex,
				 "111 -112 113 -114 (-101:102:-103:104) -1057 -1053 (1054:1052:1056:1058) ");
  makeCell("InsertWall",System,cellIndex++,wallMat,0.0,HR);

  // Frame
  const std::string strFrame=
    ModelSupport::getSeqIntersection(-51, -(50+static_cast<int>(nFrameFacets)),1);
  const HeadRule HRFrame=ModelSupport::getHeadRule(SMap,buildIndex+1000,strFrame);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"21 27 -43 (-101:102:-103:104)");
  HR *= HRFrame;
  makeCell("Frame",System,cellIndex++,wallMat,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"21 -1007 -43 (-111:112:-113:114:1057:1053)");
  HR *= HRFrame.complement();
  makeCell("FrameOuterVoid",System,cellIndex++,0,0.0,HR);

  HR = ModelSupport::getHeadRule(SMap,buildIndex,"43 -301  27 -1007");
  makeCell("FrameBackVoid",System,cellIndex++,0,0.0,HR);
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"301 327 -1007");
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

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"42 -301 -27 537 317");
  makeCell("BackFlangeOuterVoidOutsidePipe",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 317 -327");
  makeCell("BackFlangeOuterVoidOutsidePipe",System,cellIndex++,backTubePipeMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -53 -307 537 : 53 47 -307 ");
  makeCell("BackFlangeOuterVoidInsidePipe",System,cellIndex++,0,0.0,HR*backStr);

  // front tube
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-53 537 307 -317 : 53 307 -317");
  makeCell("FrontTube",System,cellIndex++,frontTubePipeMat,0.0,HR*backStr);

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

  FixedComp::setConnect(6,Origin+Y*(frontTubeLength-frontPreFlangeThick),Y);
  FixedComp::setNamedLinkSurf(6,"Cathode", SMap.realSurf(buildIndex+21));

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
