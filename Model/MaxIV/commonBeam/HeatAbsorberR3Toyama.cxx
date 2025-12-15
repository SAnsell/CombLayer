/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/HeatAbsorberR3Toyama.cxx
 *
 * Copyright (c) 2025 by Udo Friman-Gayer
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
#include <complex>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include <iostream>

#include "CFFlanges.h"
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
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "HeatAbsorberR3Toyama.h"

namespace xraySystem
{

HeatAbsorberR3Toyama::HeatAbsorberR3Toyama(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  flangeRadius(setVariable::CF63::flangeRadius)
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

HeatAbsorberR3Toyama::HeatAbsorberR3Toyama(const HeatAbsorberR3Toyama& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),flangeRadius(A.flangeRadius),
  connectorInnerRadius(A.connectorInnerRadius),absorberLength(A.absorberLength),
  absorberWidth(A.absorberWidth),absorberHeight(A.absorberHeight),
  absorberConnectorLength(A.absorberConnectorLength),
  inOutRange(A.inOutRange),closed(A.closed),
  mainMat(A.mainMat),voidMat(A.voidMat),pipeMat(A.pipeMat)
  /*!
    Copy constructor
    \param A :: HeatAbsorberR3Toyama to copy
  */
{}

HeatAbsorberR3Toyama&
HeatAbsorberR3Toyama::operator=(const HeatAbsorberR3Toyama& A)
  /*!
    Assignment operator
    \param A :: HeatAbsorberR3Toyama to copy
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
      flangeRadius=A.flangeRadius;
      connectorInnerRadius=A.connectorInnerRadius;
      absorberLength=A.absorberLength;
      absorberWidth=A.absorberWidth;
      absorberHeight=A.absorberHeight;
      absorberConnectorLength=A.absorberConnectorLength;
      inOutRange=inOutRange;
      closed=A.closed;
      mainMat=A.mainMat;
      voidMat=A.voidMat;
      pipeMat=A.pipeMat;
    }
  return *this;
}

HeatAbsorberR3Toyama*
HeatAbsorberR3Toyama::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new HeatAbsorberR3Toyama(*this);
}

HeatAbsorberR3Toyama::~HeatAbsorberR3Toyama()
  /*!
    Destructor
  */
{}

void
HeatAbsorberR3Toyama::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("HeatAbsorberR3Toyama","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  connectorInnerRadius=Control.EvalVar<double>(keyName+"ConnectorInnerRadius");
  absorberLength=Control.EvalVar<double>(keyName+"AbsorberLength");
  absorberWidth=Control.EvalVar<double>(keyName+"AbsorberWidth");
  absorberHeight=Control.EvalVar<double>(keyName+"AbsorberHeight");
  absorberConnectorLength=Control.EvalVar<double>(keyName+"AbsorberConnectorLength");
  gapWidth=Control.EvalVar<double>(keyName+"GapWidth");
  gapMinHeight=Control.EvalVar<double>(keyName+"GapMinHeight");
  gapMaxHeight=Control.EvalVar<double>(keyName+"GapMaxHeight");
  inOutRange=Control.EvalVar<double>(keyName+"InOutRange");
  closed=Control.EvalVar<int>(keyName+"Closed");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
}

void
HeatAbsorberR3Toyama::createSurfaces()
{
  ELog::RegMethod RegA("HeatAbsorberR3Toyama","createSurfaces");

  Geometry::Vec3D frontPos;
  Geometry::Vec3D backPos;

  const double zOffset = closed ? -inOutRange/2.0 : inOutRange/2.0;

  if (!frontActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Z*zOffset,Y);
    FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    frontPos = Origin;
  } else
    frontPos=ExternalCut::interPoint("front",Origin,Y);

  if (!backActive()) {
    ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
    FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    backPos = Origin+Y*length;
  } else
    backPos=ExternalCut::interPoint("back",Origin,Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*flangeRadius,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*flangeRadius,X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*flangeRadius+Z*zOffset,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*flangeRadius+Z*zOffset,Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*setVariable::CF63::flangeLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length-setVariable::CF63::flangeLength),Y);

  const double frontBackPipeLength = (length-absorberLength)/2.0;
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*frontBackPipeLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length-frontBackPipeLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+31,Origin+Y*
    (frontBackPipeLength+absorberConnectorLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+32,Origin+Y*
    (length-frontBackPipeLength-absorberConnectorLength),Y);
  ModelSupport::buildCylinder(SMap, buildIndex+7,Origin+Z*zOffset,Y,
    connectorInnerRadius+setVariable::CF63::wallThick);
  ModelSupport::buildCylinder(SMap, buildIndex+17,Origin+Z*zOffset,Y,
    connectorInnerRadius);
  ModelSupport::buildCylinder(SMap, buildIndex+27,Origin+Z*zOffset,Y,
    setVariable::CF63::flangeRadius);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*absorberWidth/2.0+Z*zOffset,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*absorberWidth/2.0+Z*zOffset,X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*absorberHeight/2.0+Z*zOffset,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*absorberHeight/2.0+Z*zOffset,Z);

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*gapWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*gapWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*gapMaxHeight/2.0+Z*zOffset,Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*gapMaxHeight/2.0+Z*zOffset,Z);

  ModelSupport::buildPlane(SMap,buildIndex+36,
    Origin+Y*frontBackPipeLength+Z*gapMaxHeight/2.0+Z*zOffset,
    Y*(gapMaxHeight-gapMinHeight)+Z*absorberLength);
}

void
HeatAbsorberR3Toyama::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("HeatAbsorberR3Toyama","createObjects");

  HeadRule HR;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  makeCell("MainCellTop",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 3 -4 -6 16"));
  makeCell("MainCellBottom",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 3 -4 5 -15"));;
  makeCell("MainCellLeft",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 3 -13 15 -16"));
  makeCell("MainCellRight",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 -4 14 15 -16"));

  makeCell("AbsorberConnectorFrontVoid",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 3 -4 5 -6 7"));
  makeCell("AbsorberConnectorFront",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 3 -4 5 -6 -7 17"));
  makeCell("AbsorberConnectorFront",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 -25 -17"));
  makeCell("AbsorberConnectorFront",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 26 -17"));
  makeCell("AbsorberConnectorFront",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 -23 -17 25 -26"));
  makeCell("AbsorberConnectorFront",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 24 -17 25 -26"));
  makeCell("AbsorberConnectorFrontSlope",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 23 -24 -26 36"));
  makeCell("AbsorberConnectorFrontHole",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -31 23 -24 25 -36"));

  makeCell("AbsorberConnectorBackVoid",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 3 -4 5 -6 7"));
  makeCell("AbsorberConnectorBack",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 3 -4 5 -6 -7 17"));
  makeCell("AbsorberConnectorBack",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 -25 -17"));
  makeCell("AbsorberConnectorBack",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 26 -17"));
  makeCell("AbsorberConnectorBack",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 -23 -17 25 -26"));
  makeCell("AbsorberConnectorBack",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 24 -17 25 -26"));
  makeCell("AbsorberConnectorBackSlope",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 23 -24 -26 36"));
  makeCell("AbsorberConnectorBackHole",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-22 32 23 -24 25 -36"));

  makeCell("AbsorberMainCellTop",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 13 -14 -16 26"));
  makeCell("AbsorberMainCellBottom",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 13 -14 15 -25"));
  makeCell("AbsorberMainCellLeft",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 13 -23 25 -26"));
  makeCell("AbsorberMainCellLeft",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 -14 24 25 -26"));

  makeCell("AbsorberSlope",System,cellIndex++,mainMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 23 -24 -26 36"));
  makeCell("AbsorberHole",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 23 -24 25 -36"));

  makeCell("FrontPipe",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -7 17"));
  makeCell("FrontPipeVoid",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 3 -4 5 -6 7"));
  makeCell("FrontPipeVoid",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -17"));
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"-11 17 -27");
  makeCell("FrontFlange",System,cellIndex++,pipeMat,0.0,HR*frontStr);
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"-11 -17");
  makeCell("FrontFlangeVoid",System,cellIndex++,voidMat,0.0,HR*frontStr);
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"-11 3 -4 5 -6 27");
  makeCell("FrontFlangeVoid",System,cellIndex++,voidMat,0.0,HR*frontStr);

  makeCell("BackPipe",System,cellIndex++,pipeMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-12 22 -7 17"));
  makeCell("BackPipeVoid",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-12 22 3 -4 5 -6 7"));
  makeCell("BackPipeVoid",System,cellIndex++,voidMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-12 22 -17"));
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"12 17 -27");
  makeCell("BackFlange",System,cellIndex++,pipeMat,0.0,HR*backStr);
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"12 -17");
  makeCell("BackFlangeVoid",System,cellIndex++,voidMat,0.0,HR*backStr);
  HR = ModelSupport::getHeadRule(SMap,buildIndex,"12 3 -4 5 -6 27");
  makeCell("BackFlangeVoid",System,cellIndex++,voidMat,0.0,HR*backStr);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 5 -6 ");
  addOuterSurf(HR*frontStr*backStr);
}


void
HeatAbsorberR3Toyama::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("HeatAbsorberR3Toyama","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*flangeRadius,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*flangeRadius,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*flangeRadius,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*flangeRadius,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
}

void
HeatAbsorberR3Toyama::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("HeatAbsorberR3Toyama","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
}

}  // xraySystem
