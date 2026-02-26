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
  flangeLength(A.flangeLength),
  flangeRadius(A.flangeRadius),
  length(A.length),
  pipeInnerRadius(A.pipeInnerRadius),
  pipeWallThick(A.pipeWallThick),

  bellowMat(A.bellowMat),
  pipeMat(A.pipeMat)
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
      flangeLength=A.flangeLength;
      flangeRadius=A.flangeRadius;
      length=A.length;
      pipeInnerRadius=A.pipeInnerRadius;
      pipeWallThick=A.pipeWallThick;

      bellowMat=A.bellowMat;
      pipeMat=A.pipeMat;
    }
  return *this;
}

CardanBellows::~CardanBellows()
{}

void
CardanBellows::populate(const FuncDataBase& Control)
{
  ELog::RegMethod RegA("CardanBellows","populate");

  angle=Control.EvalDefVar<double>(keyName+"Angle",0.0);
  bellowStep=Control.EvalVar<double>(keyName+"BellowStep");
  bellowThick=Control.EvalVar<double>(keyName+"BellowThick");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  length=Control.EvalVar<double>(keyName+"Length");
  pipeInnerRadius=Control.EvalVar<double>(keyName+"PipeInnerRadius");
  pipeWallThick=Control.EvalVar<double>(keyName+"PipeWallThick");

  bellowMat=ModelSupport::EvalDefMat(Control,keyName+"BellowMat","SteelUnknownGrade");
  pipeMat=ModelSupport::EvalDefMat(Control,keyName+"PipeMat","SteelUnknownGrade");
  return;
}

void
CardanBellows::createSurfaces()
{
  ELog::RegMethod RegA("CardanBellows","createSurfaces");

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

  ModelSupport::buildPlane(SMap,buildIndex+101,center,Yp2);

  ModelSupport::buildCylinder(SMap,buildIndex+7,center,
			      Y,flangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,center,
			      Y,pipeInnerRadius+bellowThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,center,
			      Y,pipeInnerRadius+pipeWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+37,center,
			      Y,pipeInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+107,center,
			      Yp,flangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,center,
			      Yp,pipeInnerRadius+bellowThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,center,
			      Yp,pipeInnerRadius+pipeWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+137,center,
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
    ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 -7 37"));
  makeCell("BackFlange",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"12 -2 -107 137"));

  makeCell("FrontPipe",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -27 37"));
  makeCell("FrontBellow",System,cellIndex++,bellowMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -101 -17 37"));

  makeCell("BackPipe",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"22 -12 -127 137"));
  makeCell("BackBellow",System,cellIndex++,bellowMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"101 -22 -117 137"));

  makeCell("FrontVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"1 -101 -37"));
  makeCell("FrontOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -7 27"));
  makeCell("FrontOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -101 -7 17"));

  makeCell("BackVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"101 -2 -137"));
  makeCell("BackOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"22 -12 -107 127"));
  makeCell("BackOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"101 -22 -107 117"));


  addOuterSurf(
    ModelSupport::getHeadRule(SMap,buildIndex,"(1 -101 -7) : (101 -2 -107)")
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
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
