/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/CardanBellows.cxx
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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

#include <cmath>
#include <fstream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
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
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "MaterialSupport.h"

#include "CardanBellows.h"

namespace constructSystem
{

CardanBellows::CardanBellows(const std::string& Key):
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
{}

CardanBellows::CardanBellows(const CardanBellows& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  angle(A.angle),
  bellowStep(A.bellowStep),
  bellowThick(A.bellowThick),
  bellowsVolumeFraction(A.bellowsVolumeFraction),
  flangeLength(A.flangeLength),
  flangeRadius(A.flangeRadius),
  length(A.length),
  pipeInnerRadius(A.pipeInnerRadius),
  pipeWallThick(A.pipeWallThick),

  bellowBaseMat(A.bellowBaseMat),
  pipeMat(A.pipeMat),

  bellowMat(),
  bellowsThick()
{}

CardanBellows&
CardanBellows::operator=(const CardanBellows& A)
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      angle=A.angle;
      bellowStep=A.bellowStep;
      bellowThick=A.bellowThick;
      bellowsVolumeFraction=A.bellowsVolumeFraction;
      flangeLength=A.flangeLength;
      flangeRadius=A.flangeRadius;
      length=A.length;
      pipeInnerRadius=A.pipeInnerRadius;
      pipeWallThick=A.pipeWallThick;

      bellowBaseMat=A.bellowBaseMat;
      pipeMat=A.pipeMat;
    }
  return *this;
}

CardanBellows::~CardanBellows()
{}

double CardanBellows::bellowLength() const {
  return (length-2.0*(flangeLength+bellowStep));
}

double CardanBellows::bellowsMaterialVolume() const {
  return M_PI*(
    (
      (pipeInnerRadius+bellowsMaterialThick)*(pipeInnerRadius+bellowsMaterialThick)
      -pipeInnerRadius*pipeInnerRadius
    )
    *(bellowLength()-2.0*nFolds*bellowsMaterialThick)
    +(
      (pipeInnerRadius+bellowThick)*(pipeInnerRadius+bellowThick)
      -pipeInnerRadius*pipeInnerRadius
    )*2.0*nFolds*bellowsMaterialThick
  );
}

double CardanBellows::bellowsThickness(
  const double volume, const double sLength) const {
  return sqrt(
    (
      volume*M_1_PI
      +(pipeInnerRadius+bellowsMaterialThick)*(pipeInnerRadius+bellowsMaterialThick)
      *(2.0*nFolds*bellowsMaterialThick-sLength)
      +pipeInnerRadius*pipeInnerRadius*sLength
    )/(2.0*nFolds*bellowsMaterialThick)
  )-pipeInnerRadius;
}

void CardanBellows::createSectors(){
  const double totalBellowsMaterialVolume = bellowsMaterialVolume();
  const double sBellowsMaterialVolume = totalBellowsMaterialVolume/nSectors;
  double sLength, sTotalVolume;
  for(int nSector=0;nSector<nSectors;++nSector){
    sLength = sectorLength(nSector);
    bellowsThick.push_back(bellowsThickness(totalBellowsMaterialVolume,sLength));
    sTotalVolume = (
      M_PI*(
        (pipeInnerRadius+bellowsThick[nSector])
        *(pipeInnerRadius+bellowsThick[nSector])
        -pipeInnerRadius*pipeInnerRadius
      )*sLength/nSectors
    );
    bellowMat.push_back(
      ModelSupport::EvalMatName(
        ModelSupport::EvalMatString(bellowBaseMat)
        +"%Void%"
        +std::to_string(sBellowsMaterialVolume/sTotalVolume)
      )
    );
  }
}

std::pair<int,int> CardanBellows::cylindricOuterSurf() const {
  int nSectorWithMaxRadius = 0;
  for(int n=1; n<nSectors; ++n){
    if(bellowsThick[n]>bellowsThick[nSectorWithMaxRadius]){
      nSectorWithMaxRadius = n;
    }
  }
  if(bellowsThick[nSectorWithMaxRadius] > flangeRadius){
    return {
      sectorPlaneID(nSectorWithMaxRadius,7,100),
      sectorPlaneID(nSectorWithMaxRadius,8,100)
    };
  }
  return {7,8};
}

double CardanBellows::sectorAngle(
  const int nSector,const bool centerAngle=false) const {
  if(centerAngle){
    return nSector*2.0*M_PI/nSectors;
  }
  return (nSector-0.5)*2.0*M_PI/nSectors;
}

double CardanBellows::sectorLength(const int nSector) const {
  return (bellowLength()+angle*pipeInnerRadius*cos(sectorAngle(nSector,true)));
}

int CardanBellows::sectorPlaneID(
  const int nSector, const int base, const int offset = 0
) const {
  return offset+10*(nSector%nSectors)+base;
}

void
CardanBellows::populate(const FuncDataBase& Control)
{
  ELog::RegMethod RegA("CardanBellows","populate");

  angle=Control.EvalDefVar<double>(keyName+"Angle",0.0);
  bellowsMaterialThick=Control.EvalVar<double>(keyName+"BellowsMaterialThick");
  bellowStep=Control.EvalVar<double>(keyName+"BellowStep");
  bellowThick=Control.EvalVar<double>(keyName+"BellowThick");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  length=Control.EvalVar<double>(keyName+"Length");
  pipeInnerRadius=Control.EvalVar<double>(keyName+"PipeInnerRadius");
  pipeWallThick=Control.EvalVar<double>(keyName+"PipeWallThick");
  nFolds=Control.EvalVar<int>(keyName+"NFolds");
  nSectors=Control.EvalVar<int>(keyName+"NSectors");

  bellowBaseMat=ModelSupport::EvalDefMat(
    Control,keyName+"BellowMat","SteelUnknownGrade");
  pipeMat=ModelSupport::EvalDefMat(Control,keyName+"PipeMat","SteelUnknownGrade");

  return;
}

void
CardanBellows::createSurfaces()
{
  ELog::RegMethod RegA("CardanBellows","createSurfaces");

  Geometry::Vec3D Xp = X;
  Xp.rotate(Z,angle);
  Yp = Y;
  Yp.rotate(Z,angle);
  Geometry::Vec3D Yp2 = Y;
  Yp2.rotate(Z,angle/2.0);

  const Geometry::Vec3D center = Origin+Y*length/2.0;

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,center+Yp*(length/2.0),Yp);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*flangeLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*(flangeLength+bellowStep),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
    center+Yp*(length/2.0-flangeLength),Yp);
  ModelSupport::buildPlane(SMap,buildIndex+22,
    center+Yp*(length/2.0-flangeLength-bellowStep),Yp);

  double phi;
  for(int n = 0; n < nSectors; ++n){
    phi = sectorAngle(n);
    ModelSupport::buildPlane(
      SMap,buildIndex+sectorPlaneID(n,3),Origin,X*sin(phi)+Z*cos(phi)
    );
    ModelSupport::buildPlane(
      SMap,buildIndex+sectorPlaneID(n,4),Origin,Xp*sin(phi)+Z*cos(phi)
    );
    ModelSupport::buildCylinder(SMap,buildIndex+sectorPlaneID(n,7,100),center,
              Y,pipeInnerRadius+bellowsThick[n]);
    ModelSupport::buildCylinder(SMap,buildIndex+sectorPlaneID(n,8,100),center,
              Yp,pipeInnerRadius+bellowsThick[n]);
  }

  ModelSupport::buildPlane(SMap,buildIndex+101,center,Yp2);

  ModelSupport::buildCylinder(SMap,buildIndex+7,center,
			      Y,flangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,center,
			      Y,pipeInnerRadius+pipeWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,center,
			      Y,pipeInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+8,center,
			      Yp,flangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+18,center,
			      Yp,pipeInnerRadius+pipeWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+28,center,
			      Yp,pipeInnerRadius);
}

void
CardanBellows::createObjects(Simulation& System)
{
  ELog::RegMethod RegA("CardanBellows","createObjects");

  HeadRule HR;

  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  makeCell("FrontFlange",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 -7 27"));
  makeCell("BackFlange",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"12 -2 -8 28"));

  makeCell("FrontPipe",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -17 27"));

  std::pair<int,int> cylOuterSurf = cylindricOuterSurf();
  for(int n = 0; n < nSectors; ++n){
    makeCell("FrontBellow",System,cellIndex++,bellowMat[n],0.0,
      ModelSupport::getHeadRule(SMap,buildIndex,"21 -101 27")
      *ModelSupport::getHeadRule(
        SMap,buildIndex,std::to_string(sectorPlaneID(n,3))).complement()
      *ModelSupport::getHeadRule(SMap,buildIndex,std::to_string(sectorPlaneID(n+1,3)))
      *ModelSupport::getHeadRule(
        SMap,buildIndex,std::to_string(sectorPlaneID(n,7,100))).complement()
    );

    makeCell("BackBellow",System,cellIndex++,bellowMat[n],0.0,
      ModelSupport::getHeadRule(SMap,buildIndex,"101 -22 28")
      *ModelSupport::getHeadRule(
        SMap,buildIndex,std::to_string(sectorPlaneID(n,4))).complement()
      *ModelSupport::getHeadRule(SMap,buildIndex,std::to_string(sectorPlaneID(n+1,4)))
      *ModelSupport::getHeadRule(
        SMap,buildIndex,std::to_string(sectorPlaneID(n,8,100))).complement()
    );
  }

  makeCell("BackPipe",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"22 -12 -18 28"));

  makeCell("FrontVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"1 -101 -27"));
  makeCell("FrontOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -7 17"));

  makeCell("BackVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"101 -2 -28"));
  makeCell("BackOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"22 -12 -8 18"));

  addOuterSurf(
    ModelSupport::getHeadRule(SMap,buildIndex,"-101")
    *ModelSupport::getHeadRule(
      SMap,buildIndex,std::to_string(cylOuterSurf.first)).complement()
    *frontHR
    +ModelSupport::getHeadRule(SMap,buildIndex,"101")
    *ModelSupport::getHeadRule(
      SMap,buildIndex,std::to_string(cylOuterSurf.second)).complement()
    *backHR
  );
}

void
CardanBellows::createLinks()
{
  ELog::RegMethod RegA("Bellow","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*length/2.0+Yp*length/2.0,Yp);

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}

void
CardanBellows::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int FIndex)
{
  ELog::RegMethod RegA("CardanBellows","createAll(FC)");

  populate(System.getDataBase());
  createSectors();
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
