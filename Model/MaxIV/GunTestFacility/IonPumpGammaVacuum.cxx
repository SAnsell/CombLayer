/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/IonPumpGammaVacuum.cxx
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

#include "IonPumpGammaVacuum.h"

namespace MAXIV
{

IonPumpGammaVacuum::IonPumpGammaVacuum(const std::string& Key)  :
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

IonPumpGammaVacuum::IonPumpGammaVacuum(const IonPumpGammaVacuum& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),height(A.height),
  wallThick(A.wallThick),
  zClearance(A.zClearance),
  pistonWidth(A.pistonWidth),
  pistonHeight(A.pistonHeight),
  pistonBaseHeight(A.pistonBaseHeight),
  pistonBaseThick(A.pistonBaseThick),
  pistonLength(A.pistonLength),
  flangeRadius(A.flangeRadius),
  flangeThick(A.flangeThick),
  flangeTubeRadius(A.flangeTubeRadius),
  flangeTubeThick(A.flangeTubeThick),
  flangeTubeLength(A.flangeTubeLength),
  mainMat(A.mainMat),wallMat(A.wallMat),
  pistonMat(A.pistonMat)
  /*!
    Copy constructor
    \param A :: IonPumpGammaVacuum to copy
  */
{}

IonPumpGammaVacuum&
IonPumpGammaVacuum::operator=(const IonPumpGammaVacuum& A)
  /*!
    Assignment operator
    \param A :: IonPumpGammaVacuum to copy
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
      height=A.height;
      wallThick=A.wallThick;
      zClearance=A.zClearance;
      pistonWidth=A.pistonWidth;
      pistonHeight=A.pistonHeight;
      pistonBaseHeight=A.pistonBaseHeight;
      pistonBaseThick=A.pistonBaseThick;
      pistonLength=A.pistonLength;
      flangeRadius=A.flangeRadius;
      flangeThick=A.flangeThick;
      flangeTubeRadius=A.flangeTubeRadius;
      flangeTubeThick=A.flangeTubeThick;
      flangeTubeLength=A.flangeTubeLength;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      pistonMat=A.pistonMat;
    }
  return *this;
}

IonPumpGammaVacuum*
IonPumpGammaVacuum::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new IonPumpGammaVacuum(*this);
}

IonPumpGammaVacuum::~IonPumpGammaVacuum()
  /*!
    Destructor
  */
{}

void
IonPumpGammaVacuum::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("IonPumpGammaVacuum","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  zClearance=Control.EvalDefVar<double>(keyName+"ZClearance",1.0);
  pistonWidth=Control.EvalVar<double>(keyName+"PistonWidth");
  pistonHeight=Control.EvalVar<double>(keyName+"PistonHeight");
  pistonBaseHeight=Control.EvalVar<double>(keyName+"PistonBaseHeight");
  pistonBaseThick=Control.EvalVar<double>(keyName+"PistonBaseThick");
  pistonLength=Control.EvalVar<double>(keyName+"PistonLength");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeThick=Control.EvalVar<double>(keyName+"FlangeThick");
  flangeTubeRadius=Control.EvalVar<double>(keyName+"FlangeTubeRadius");
  flangeTubeThick=Control.EvalVar<double>(keyName+"FlangeTubeThick");
  flangeTubeLength=Control.EvalVar<double>(keyName+"FlangeTubeLength");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  pistonMat=ModelSupport::EvalMat<int>(Control,keyName+"PistonMat");

  return;
}

void
IonPumpGammaVacuum::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("IonPumpGammaVacuum","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(wallThick),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      wallThick);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length+flangeTubeLength),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+12));

      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length+flangeTubeLength-flangeThick),Y);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -flangeThick);
    }

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0-wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0-wallThick),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(pistonWidth/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(pistonWidth/2.0+wallThick),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height/2.0),Z);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+21,buildIndex+12,Y,-flangeTubeLength-pistonBaseThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+22,buildIndex+21,Y,pistonBaseThick);

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(pistonWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(pistonWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(pistonBaseHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(pistonBaseHeight/2.0),Z);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+31,buildIndex+22,Y,-pistonLength);

  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*(pistonHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*(pistonHeight/2.0),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flangeTubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,flangeTubeRadius+flangeTubeThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeRadius);

  ModelSupport::buildPlane(SMap,buildIndex+45,Origin-Z*(flangeRadius+zClearance),Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,Origin+Z*(flangeRadius+zClearance),Z);


  return;
}

void
IonPumpGammaVacuum::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("IonPumpGammaVacuum","createObjects");

  HeadRule Out;
  const HeadRule frontStr(frontRule());
  const HeadRule backStr(backRule());

  Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -21 23 -24 5 -6 (-23:24:-35:36:-31)");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," 31 -21  23 -24 35 -36 ");
  makeCell("Piston",System,cellIndex++,pistonMat,0.0,Out);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," 21 -22 23 -24 25 -26 ");
  makeCell("PistonBase",System,cellIndex++,pistonMat,0.0,Out);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," 21 -22 13 -14 15 -16 (-23:24:-25:26) ");
  makeCell("PistonBaseOuter",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," 22 -7 ");
  makeCell("Flange",System,cellIndex++,mainMat,0.0,Out*backStr);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," 22 -2 7 -17 ");
  makeCell("FlangeWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 7 -27");
  makeCell("FlangeConnect",System,cellIndex++,wallMat,0.0,Out*backStr);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 13 -14 15 -16 27");
  makeCell("FlangeConnectVoid",System,cellIndex++,0,0.0,Out*backStr);


  Out=ModelSupport::getHeadRule(SMap,buildIndex," 22 -2 13 -14 15 -16 17 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," -1 13 -14 15 -16 ");
  makeCell("WallBack",System,cellIndex++,wallMat,0.0,Out*frontStr);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," -2 13 -14 45 -15");
  makeCell("ZClearanceBottom",System,cellIndex++,mainMat,0.0,Out*frontStr);
  Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 13 -14 45 -15 27");
  makeCell("ZClearanceBottom",System,cellIndex++,mainMat,0.0,Out*backStr);

  Out=ModelSupport::getHeadRule(SMap,buildIndex," -2 13 -14 16 -46");
  makeCell("ZClearanceTop",System,cellIndex++,mainMat,0.0,Out*frontStr);
  Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 13 -14 16 -46 27");
  makeCell("ZClearanceTop",System,cellIndex++,mainMat,0.0,Out*backStr);


  Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -21 13 -14 15 -16 (-1:22:-23:24:-5:6) ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);


  Out=ModelSupport::getHeadRule(SMap,buildIndex," 13 -14 45 -46");
  addOuterSurf(Out*frontStr*backStr);

  return;
}


void
IonPumpGammaVacuum::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("IonPumpGammaVacuum","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  // FixedComp::setConnect(2,Origin-X*(pistonWidth/2.0+wallThick),-X);
  // FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  // FixedComp::setConnect(3,Origin+X*(pistonWidth/2.0+wallThick),X);
  // FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
IonPumpGammaVacuum::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("IonPumpGammaVacuum","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // MAXIV
