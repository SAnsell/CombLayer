/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/IonChamber.cxx
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

#include "IonChamber.h"

namespace xraySystem
{

IonChamber::IonChamber(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

IonChamber::IonChamber(const IonChamber& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  radius(A.radius),
  cRadius(A.cRadius),
  height(A.height),
  wRadius(A.wRadius),
  wThick(A.wThick),
  wHeight(A.wHeight),
  wOffset(A.wOffset),
  cOffset(A.cOffset),
  heRadius(A.heRadius),
  heWallThick(A.heWallThick),
  heHeight(A.heHeight),
  rubberThick(A.rubberThick),
  bottomInsulatorRadius(A.bottomInsulatorRadius),
  bottomInsulatorHeight(A.bottomInsulatorHeight),
  topInsulatorRadius(A.topInsulatorRadius),
  topInsulatorHeight(A.topInsulatorHeight),
  modMat(A.modMat),
  wMat(A.wMat),
  heWallMat(A.heWallMat),
  heMat(A.heMat),
  airMat(A.airMat),
  rubberMat(A.rubberMat)
  /*!
    Copy constructor
    \param A :: IonChamber to copy
  */
{}

IonChamber&
IonChamber::operator=(const IonChamber& A)
  /*!
    Assignment operator
    \param A :: IonChamber to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      radius=A.radius;
      cRadius=A.cRadius;
      height=A.height;
      wRadius=A.wRadius;
      wThick=A.wThick;
      wHeight=A.wHeight;
      wOffset=A.wOffset;
      cOffset=A.cOffset;
      heRadius=A.heRadius;
      heWallThick=A.heWallThick;
      heHeight=A.heHeight;
      rubberThick=A.rubberThick;
      bottomInsulatorRadius=A.bottomInsulatorRadius;
      bottomInsulatorHeight=A.bottomInsulatorHeight;
      topInsulatorRadius=A.topInsulatorRadius;
      topInsulatorHeight=A.topInsulatorHeight;
      modMat=A.modMat;
      wMat=A.wMat;
      heWallMat=A.heWallMat;
      heMat=A.heMat;
      airMat=A.airMat;
      rubberMat=A.rubberMat;
    }
  return *this;
}

IonChamber*
IonChamber::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new IonChamber(*this);
}

IonChamber::~IonChamber()
  /*!
    Destructor
  */
{}

void
IonChamber::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("IonChamber","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  cRadius=Control.EvalVar<double>(keyName+"CounterTubeRadius");
  height=Control.EvalVar<double>(keyName+"Height");
  wRadius=Control.EvalVar<double>(keyName+"TungstenRadius");
  wThick=Control.EvalVar<double>(keyName+"TungstenThick");
  wHeight=Control.EvalVar<double>(keyName+"TungstenHeight");
  wOffset=Control.EvalVar<double>(keyName+"TungstenOffset");
  cOffset=Control.EvalVar<double>(keyName+"CounterOffset");
  heRadius=Control.EvalVar<double>(keyName+"HeRadius");
  heWallThick=Control.EvalVar<double>(keyName+"HeWallThick");
  heHeight=Control.EvalVar<double>(keyName+"HeHeight");
  rubberThick=Control.EvalVar<double>(keyName+"RubberThick");
  bottomInsulatorRadius=Control.EvalVar<double>(keyName+"BottomInsulatorRadius");
  bottomInsulatorHeight=Control.EvalVar<double>(keyName+"BottomInsulatorHeight");
  topInsulatorRadius=Control.EvalVar<double>(keyName+"TopInsulatorRadius");
  topInsulatorHeight=Control.EvalVar<double>(keyName+"TopInsulatorHeight");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModeratorMat");
  wMat=ModelSupport::EvalMat<int>(Control,keyName+"TungstenMat");
  heWallMat=ModelSupport::EvalMat<int>(Control,keyName+"HeWallMat");
  heMat=ModelSupport::EvalMat<int>(Control,keyName+"HeMat");
  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");
  rubberMat=ModelSupport::EvalMat<int>(Control,keyName+"RubberMat");

  return;
}

void
IonChamber::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("IonChamber","createSurfaces");

  SurfMap::makePlane("bottom",SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
  SurfMap::makeCylinder("CounterCyl",SMap,buildIndex+7,Origin,Z,cRadius);

  SurfMap::makePlane("TungstenFloorUpper",SMap,buildIndex+15,Origin-Z*(height/2.0-wOffset-wThick),Z);
  SurfMap::makeShiftedPlane("ChamberFloor",SMap,buildIndex+16,buildIndex+15,Z,cOffset);
  SurfMap::makePlane("TungstenFloorLower",SMap,buildIndex+25,Origin-Z*(height/2.0-wOffset),Z);
  SurfMap::makeShiftedPlane("TungstenUpper",SMap,buildIndex+26,buildIndex+25,Z,wHeight);
  SurfMap::makeCylinder("TungstenInnerCyl",SMap,buildIndex+17,Origin,Z,wRadius);
  SurfMap::makeCylinder("TungstenOuterCyl",SMap,buildIndex+27,Origin,Z,wRadius+wThick);

  SurfMap::makeShiftedPlane("HeFloor",SMap,buildIndex+36,buildIndex+16,Z,heWallThick);
  SurfMap::makeShiftedPlane("HeRoofLower",SMap,buildIndex+45,buildIndex+36,Z,heHeight);
  SurfMap::makeShiftedPlane("HeRoofUpper",SMap,buildIndex+46,buildIndex+45,Z,heWallThick);

  SurfMap::makeCylinder("ModeratorCyl",SMap,buildIndex+37,Origin,Z,radius);

  SurfMap::makeCylinder("HeCylInner",SMap,buildIndex+47,Origin,Z,heRadius);
  SurfMap::makeCylinder("HeCylOuter",SMap,buildIndex+57,Origin,Z,heRadius+heWallThick);

  SurfMap::makeShiftedPlane("BottomInsulatorRoof",SMap,buildIndex+66,buildIndex+16,Z,
			    bottomInsulatorHeight);
  SurfMap::makeCylinder("BottomInsulatorCyl",SMap,buildIndex+67,Origin,Z,bottomInsulatorRadius);
  SurfMap::makeShiftedPlane("TopInsulatorFloor",SMap,buildIndex+75,buildIndex+46,Z,
			    -topInsulatorHeight);
  SurfMap::makeCylinder("TopInsulatorCyl",SMap,buildIndex+77,Origin,Z,topInsulatorRadius);

  SurfMap::makeShiftedPlane("RubberFloor",SMap,buildIndex+85,buildIndex+6,Z,-rubberThick);

  return;
}

void
IonChamber::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("IonChamber","createObjects");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -47 (66 -75 : 67 36 -66 : 77 75 -45) ");
  makeCell("He3",System,cellIndex++,heMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 47 -57 36 -45 ");
  makeCell("HeSideWall",System,cellIndex++,heWallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 57 -7 16 -46 ");
  makeCell("HeSideWallAir",System,cellIndex++,airMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 77 -57 45 -46 ");
  makeCell("HeRoof",System,cellIndex++,heWallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 46 -85 ");
  makeCell("Chamber",System,cellIndex++,airMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 85 -6 ");
  makeCell("Rubber",System,cellIndex++,rubberMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 67 -57 16 -36 ");
  makeCell("HeFloor",System,cellIndex++,heWallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -67 16 -66 ");
  makeCell("BottomInsulator",System,cellIndex++,modMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -77 75 -46 ");
  makeCell("TopInsulator",System,cellIndex++,modMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex," -17 15 -16 ");
  makeCell("ModeratorBelowChamber",System,cellIndex++,modMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 7 -17 16 -26 ");
  makeCell("ModeratorInner",System,cellIndex++,modMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 17 -27 25 -26 ");
  makeCell("TungstenSide",System,cellIndex++,wMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -17 25 -15 ");
  makeCell("TungstenBottom",System,cellIndex++,wMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 27 -37 25 -26 ");
  makeCell("ModeratorSide",System,cellIndex++,modMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 7 -37 26 -6 ");
  makeCell("ModeratorTop",System,cellIndex++,modMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -37 5 -25 ");
  makeCell("ModeratorBottom",System,cellIndex++,modMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -37 5 -6 ");
  addOuterSurf(HR);

  return;
}


void
IonChamber::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("IonChamber","createLinks");

  FixedComp::setConnect(0,Origin-Z*(height/2.0),-Z);
  FixedComp::setNamedLinkSurf(0,"bottom",SurfMap::getSignedSurf("#bottom"));

 // TODO: Check and use names for the links below:

 //  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
 //  FixedComp::setNamedLinkSurf(1,"Front",SMap.realSurf(buildIndex+2));

 //  FixedComp::setConnect(2,Origin-X*(radius/2.0),-X);
 //  FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

 //  FixedComp::setConnect(3,Origin+X*(radius/2.0),X);
 //  FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+4));

 //  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
 //  FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+5));

 //  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
 //  FixedComp::setNamedLinkSurf(5,"Top",SMap.realSurf(buildIndex+6));

  return;
}

void
IonChamber::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("IonChamber","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
