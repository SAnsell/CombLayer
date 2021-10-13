/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/TDCBeamDump.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#include "BaseVisit.h"
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

#include "TDCBeamDump.h"

namespace tdcSystem
{

TDCBeamDump::TDCBeamDump(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TDCBeamDump::TDCBeamDump(const TDCBeamDump& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::ExternalCut(A),
  length(A.length),
  bulkWidthLeft(A.bulkWidthLeft),
  bulkWidthRight(A.bulkWidthRight),
  bulkHeight(A.bulkHeight),
  bulkDepth(A.bulkDepth),
  bulkThickBack(A.bulkThickBack),
  coreRadius(A.coreRadius),
  coreLength(A.coreLength),
  preCoreLength(A.preCoreLength),
  skinThick(A.skinThick),
  coreMat(A.coreMat),
  bulkMat(A.bulkMat),
  skinMat(A.skinMat)
  /*!
    Copy constructor
    \param A :: TDCBeamDump to copy
  */
{}

TDCBeamDump&
TDCBeamDump::operator=(const TDCBeamDump& A)
  /*!
    Assignment operator
    \param A :: TDCBeamDump to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      bulkWidthLeft=A.bulkWidthLeft;
      bulkWidthRight=A.bulkWidthRight;
      bulkHeight=A.bulkHeight;
      bulkDepth=A.bulkDepth;
      bulkThickBack=A.bulkThickBack;
      coreRadius=A.coreRadius;
      coreLength=A.coreLength;
      preCoreLength=A.preCoreLength;
      skinThick=A.skinThick;
      coreMat=A.coreMat;
      bulkMat=A.bulkMat;
      skinMat=A.skinMat;
    }
  return *this;
}

TDCBeamDump*
TDCBeamDump::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new TDCBeamDump(*this);
}

TDCBeamDump::~TDCBeamDump()
  /*!
    Destructor
  */
{}

void
TDCBeamDump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("TDCBeamDump","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  bulkWidthLeft=Control.EvalVar<double>(keyName+"BulkWidthLeft");
  bulkWidthRight=Control.EvalVar<double>(keyName+"BulkWidthRight");
  bulkHeight=Control.EvalVar<double>(keyName+"BulkHeight");
  bulkDepth=Control.EvalVar<double>(keyName+"BulkDepth");
  bulkThickBack=Control.EvalVar<double>(keyName+"BulkThickBack");
  coreRadius=Control.EvalVar<double>(keyName+"CoreRadius");
  coreLength=Control.EvalVar<double>(keyName+"CoreLength");
  preCoreLength=Control.EvalVar<double>(keyName+"PreCoreLength");
  skinThick=Control.EvalVar<double>(keyName+"SkinThick");

  coreMat=ModelSupport::EvalMat<int>(Control,keyName+"CoreMat");
  bulkMat=ModelSupport::EvalMat<int>(Control,keyName+"BulkMat");
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");

  return;
}

void
TDCBeamDump::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TDCBeamDump","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*skinThick,Y);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+11,buildIndex+1,Y,-skinThick);

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(preCoreLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(preCoreLength+coreLength),Y);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+22,buildIndex+12,Y,bulkThickBack);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+32,buildIndex+22,Y,skinThick);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(bulkWidthLeft),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+13,buildIndex+3,X,-skinThick);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(bulkWidthRight),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+14,buildIndex+4,X,skinThick);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(bulkDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(bulkHeight),Z);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+15,buildIndex+5,Z,-skinThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+16,buildIndex+6,Z,skinThick);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,coreRadius);

  return;
}

void
TDCBeamDump::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TDCBeamDump","createObjects");

  const HeadRule& frontHR=ExternalCut::getRule("front");
  const HeadRule& backHR=ExternalCut::getRule("back");
  const HeadRule& baseHR=ExternalCut::getRule("base");

  HeadRule Out;
  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -2 -7 ");
  makeCell("PreCore",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 -7 ");
  makeCell("Core",System,cellIndex++,coreMat,0.0,Out);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"1 -12 3 -4 5 -6 7 ");
  makeCell("Bulk",System,cellIndex++,bulkMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"12 -22 3 -4 5 -6 ");
  makeCell("Bulk",System,cellIndex++,bulkMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 3 -4 5 -6 7 ");
  makeCell("SkinFont",System,cellIndex++,skinMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 13 -14 6 -16 ");
  makeCell("SkinTop",System,cellIndex++,skinMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 13 -14 15 -5 ");
  makeCell("SkinBottom",System,cellIndex++,skinMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"22 -32 3 -4 5 -6 ");
  makeCell("SkinBack",System,cellIndex++,skinMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 13 -3 5 -6 ");
  makeCell("SkinLeft",System,cellIndex++,skinMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 4 -14 5 -6 ");
  makeCell("SkinRight",System,cellIndex++,skinMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 13 -14 15 -16");
  addOuterSurf(Out);

  return;
}


void
TDCBeamDump::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("TDCBeamDump","createLinks");

  const double totalLength = preCoreLength + coreLength + bulkThickBack + skinThick;

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(1,Origin+Y*(totalLength),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+32));

  FixedComp::setConnect(2,Origin-X*(bulkWidthLeft+skinThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));

  FixedComp::setConnect(3,Origin+X*(bulkWidthRight+skinThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(4,Origin-Z*(bulkDepth+skinThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(5,Origin+Z*(bulkHeight+skinThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}

void
TDCBeamDump::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("TDCBeamDump","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
