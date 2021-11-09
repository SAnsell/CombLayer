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
#include "OutputLog.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "SurfMap.h"

#include "NameStack.h"
#include "RegMethod.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "AttachSupport.h"

#include "TDCBeamDump.h"

namespace tdcSystem
{

TDCBeamDump::TDCBeamDump(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotateGroup(Key,"Main",6,"Beam",4),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  mainFC1(nullptr),
  mainFCSide(0)

 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TDCBeamDump::TDCBeamDump(const TDCBeamDump& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotateGroup(A),
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
  preCoreRadius(A.preCoreRadius),
  skinThick(A.skinThick),
  frontPlateThick(A.frontPlateThick),
  carbonThick(A.carbonThick),
  coreMat(A.coreMat),
  bulkMat(A.bulkMat),
  skinMat(A.skinMat),
  frontPlateMat(A.frontPlateMat),
  carbonMat(A.carbonMat),
  mainFC1(A.mainFC1),
  mainFCSide(A.mainFCSide)
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
      attachSystem::FixedRotateGroup::operator=(A);
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
      preCoreRadius=A.preCoreRadius;
      skinThick=A.skinThick;
      frontPlateThick=A.frontPlateThick;
      carbonThick=A.carbonThick;
      coreMat=A.coreMat;
      bulkMat=A.bulkMat;
      skinMat=A.skinMat;
      frontPlateMat=A.frontPlateMat;
      carbonMat=A.carbonMat;
      mainFC1=A.mainFC1;
      mainFCSide=A.mainFCSide;
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

  FixedRotateGroup::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  bulkWidthLeft=Control.EvalVar<double>(keyName+"BulkWidthLeft");
  bulkWidthRight=Control.EvalVar<double>(keyName+"BulkWidthRight");
  bulkHeight=Control.EvalVar<double>(keyName+"BulkHeight");
  bulkDepth=Control.EvalVar<double>(keyName+"BulkDepth");
  bulkThickBack=Control.EvalVar<double>(keyName+"BulkThickBack");
  coreRadius=Control.EvalVar<double>(keyName+"CoreRadius");
  coreLength=Control.EvalVar<double>(keyName+"CoreLength");
  preCoreLength=Control.EvalVar<double>(keyName+"PreCoreLength");
  preCoreRadius=Control.EvalVar<double>(keyName+"PreCoreRadius");
  skinThick=Control.EvalVar<double>(keyName+"SkinThick");
  frontPlateThick=Control.EvalVar<double>(keyName+"FrontPlateThick");
  carbonThick=Control.EvalVar<double>(keyName+"CarbonThick");

  coreMat=ModelSupport::EvalMat<int>(Control,keyName+"CoreMat");
  bulkMat=ModelSupport::EvalMat<int>(Control,keyName+"BulkMat");
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  frontPlateMat=ModelSupport::EvalMat<int>(Control,keyName+"FrontPlateMat");
  carbonMat=ModelSupport::EvalMat<int>(Control,keyName+"CarbonMat");

  return;
}

void
TDCBeamDump::createUnitVector(const attachSystem::FixedComp& centreFC,
			   const long int cIndex,
			   const attachSystem::FixedComp& pipeFC,
			   const long int pIndex)
  /*!
    Create the unit vectors.
    The first beamFC is to set the X,Y,Z relative to the beam
    and the origin at the beam centre position.

    \param centreFC :: FixedComp for origin
    \param cIndex :: link point of centre [and axis]
    \param pipeFC :: link point of pipe centre
    \param pIndex :: direction for links
  */
{
  ELog::RegMethod RegA("TDCBeamDump","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  mainFC.createUnitVector(centreFC,cIndex);
  beamFC.createUnitVector(pipeFC,pIndex);

  applyOffset();
  // if (upFlag)
  //   beamFC.applyShift(0,0,lift);  // only beam offset
  setDefault("Beam");
  setSecondary("Main");
  return;
}


void
TDCBeamDump::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TDCBeamDump","createSurfaces");

  //  setDefault("Beam", "Main");

  // if (!mainFC1)
  //   bZ = mainFC1->getZ();
  // else
  bY = Geometry::Vec3D(0,1,0);
  bZ = Geometry::Vec3D(0,0,1);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+bY*skinThick,bY);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+11,buildIndex+1,bY,-skinThick);

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(preCoreLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(preCoreLength+coreLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+bY*(preCoreLength+coreLength+bulkThickBack),bY);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+32,buildIndex+22,bY,skinThick);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(bulkWidthLeft),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+13,buildIndex+3,X,-skinThick);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(bulkWidthRight),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+14,buildIndex+4,X,skinThick);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-bZ*(bulkDepth),bZ);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+bZ*(bulkHeight),bZ);

  ELog::EM << keyName << " createSurfaces: Z: " << Z << " bZ: " << bZ << ELog::endDiag;

  ModelSupport::buildShiftedPlane(SMap,buildIndex+15,buildIndex+5,bZ,-skinThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+16,buildIndex+6,bZ,skinThick);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+41,buildIndex+1,bY,frontPlateThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+42,buildIndex+2,Y,-carbonThick);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,preCoreRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,coreRadius);

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
  // const HeadRule& backHR=ExternalCut::getRule("back");
  const HeadRule& baseHR=ExternalCut::getRule("base");
  //  const HeadRule& topHR=ExternalCut::getRule("top");

  HeadRule Out;
  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -42 -7 ");
  makeCell("PreCore",System,cellIndex++,0,0.0,Out*frontHR);
  Out=ModelSupport::getHeadRule(SMap,buildIndex,"42 -2 -7 ");
  makeCell("Carbon",System,cellIndex++,carbonMat,0.0,Out);
  Out=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 -17 ");
  makeCell("Core",System,cellIndex++,coreMat,0.0,Out);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"1 -41 3 -4 5 -6 7 ");
  makeCell("FrontPlate",System,cellIndex++,frontPlateMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"41 -2 3 -4 5 -6 7 ");
  makeCell("Bulk",System,cellIndex++,bulkMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 3 -4 5 -6 17 ");
  makeCell("Bulk",System,cellIndex++,bulkMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"12 -22 3 -4 5 -6 ");
  makeCell("Bulk",System,cellIndex++,bulkMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 3 -4 5 -6 7 ");
  makeCell("SkinFront",System,cellIndex++,skinMat,0.0,Out*baseHR*frontHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 13 -14 6 -16 ");
  makeCell("SkinTop",System,cellIndex++,skinMat,0.0,Out);

  if (baseHR.isEmpty())
    {
      Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 13 -14 15 -5 ");
      makeCell("SkinBottom",System,cellIndex++,skinMat,0.0,Out*baseHR);
    }

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"22 -32 3 -4 5 -6 ");
  makeCell("SkinBack",System,cellIndex++,skinMat,0.0,Out*baseHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 13 -3 5 -6 ");
  makeCell("SkinLeft",System,cellIndex++,skinMat,0.0,Out*baseHR*frontHR);

  Out=ModelSupport::getHeadRule(SMap,buildIndex,"11 -32 4 -14 5 -6 ");
  makeCell("SkinRight",System,cellIndex++,skinMat,0.0,Out*baseHR*frontHR);

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

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  mainFC.setConnect(0,Origin,-Y);
  mainFC.setLinkSurf(0,-SMap.realSurf(buildIndex+11));

  mainFC.setConnect(1,Origin+Y*(totalLength),Y);
  mainFC.setLinkSurf(1,SMap.realSurf(buildIndex+32));

  mainFC.setConnect(2,Origin-X*(bulkWidthLeft+skinThick),-X);
  mainFC.setLinkSurf(2,-SMap.realSurf(buildIndex+13));

  mainFC.setConnect(3,Origin+X*(bulkWidthRight+skinThick),X);
  mainFC.setLinkSurf(3,SMap.realSurf(buildIndex+14));

  mainFC.setConnect(4,Origin-Z*(bulkDepth+skinThick),-Z);
  mainFC.setLinkSurf(4,-SMap.realSurf(buildIndex+15));

  mainFC.setConnect(5,Origin+Z*(bulkHeight+skinThick),Z);
  mainFC.setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}

void
TDCBeamDump::setMainAxis(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
/*!
  Main axis setter
  \param FC :: Main axis fixed component (origin)
  \param sideIndex :: link point for origin
 */
{
  mainFC1 = const_cast<attachSystem::FixedComp*>(&FC);
  mainFCSide = sideIndex;//attachSystem::getLinkNumber(sideName);
}

void
TDCBeamDump::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin (pipe)
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("TDCBeamDump","createAll");

  if (!mainFC1)
    {
      mainFC1 = const_cast<attachSystem::FixedComp*>(&FC);
      mainFCSide = sideIndex;
    }

  populate(System.getDataBase());
  // ELog::EM << keyName << " createAll: mainFC1: " << mainFC1->getZ() << ELog::endDiag;
  // ELog::EM << keyName << " createAll: beam: " << getKey("Beam").getZ() << ELog::endDiag;
  createUnitVector(*mainFC1,mainFCSide, FC,sideIndex);
  // ELog::EM << keyName << " createAll: mainFC1: " << mainFC1->getZ() << ELog::endDiag;
  // ELog::EM << keyName << " createAll: beam: " << getKey("Beam").getZ() << ELog::endDiag;
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
