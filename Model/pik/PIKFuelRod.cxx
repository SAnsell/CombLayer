/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pik/PIKFuelRod.cxx
 *
 * Copyright (c) 2004-2022 by SSY
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

#include "PIKFuelRod.h"

namespace pikSystem
{

PIKFuelRod::PIKFuelRod(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PIKFuelRod::PIKFuelRod(const PIKFuelRod& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  height(A.height),
  outerR(A.outerR),
  radius(A.radius),
  thickenR(A.thickenR),
  mainMat(A.mainMat)
  /*!
    Copy constructor
    \param A :: PIKFuelRod to copy
  */
{}

PIKFuelRod&
PIKFuelRod::operator=(const PIKFuelRod& A)
  /*!
    Assignment operator
    \param A :: PIKFuelRod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      height=A.height;
      outerR=A.outerR;
      radius=A.radius;
      thickenR=A.thickenR;
      mainMat=A.mainMat;
    }
  return *this;
}

PIKFuelRod*
PIKFuelRod::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PIKFuelRod(*this);
}

PIKFuelRod::~PIKFuelRod()
  /*!
    Destructor
  */
{}

void
PIKFuelRod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PIKFuelRod","populate");

  FixedRotate::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  outerR=Control.EvalVar<double>(keyName+"OuterRadius");
  radius=Control.EvalVar<double>(keyName+"Radius");
  thickenR=Control.EvalVar<double>(keyName+"ThickenRadius");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  return;
}

void
PIKFuelRod::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Z,outerR);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(radius),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(radius),Y);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(outerR-radius),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(outerR-radius),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(radius),X);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(outerR-radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(outerR-radius),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin-Y*(outerR-radius),Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin+Y*(outerR-radius),Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+9,Origin-X*(outerR-radius),Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+10,Origin+X*(outerR-radius),Z,radius);

  double thickenCenter = radius+thickenR;
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin-Y*(thickenCenter)+X*(thickenCenter),Z,thickenR);
  ModelSupport::buildCylinder(SMap,buildIndex+108,Origin+Y*(thickenCenter)+X*(thickenCenter),Z,thickenR);
  ModelSupport::buildCylinder(SMap,buildIndex+109,Origin-Y*(thickenCenter)-X*(thickenCenter),Z,thickenR);
  ModelSupport::buildCylinder(SMap,buildIndex+110,Origin+Y*(thickenCenter)-X*(thickenCenter),Z,thickenR);

  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(thickenCenter),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(thickenCenter),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(thickenCenter),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(thickenCenter),X);

  return;
}

void
PIKFuelRod::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createObjects");

  std::string Out;

  //HeadRule Out;
  //Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  //makeCell("MainCell",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 3 -4 5 -6");
  makeCell("MainCell1",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 13 -3 5 -6");
  makeCell("MainCell2",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 4 -14 5 -6");
  makeCell("MainCell3",System,cellIndex++,mainMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," -1007 7 3 -4 -11 5 -6");
  makeCell("VoidW",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -11 5 -6");
  makeCell("TipW",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1007 8 3 -4 12 5 -6");
  makeCell("VoidE",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -8 12 5 -6");
  makeCell("TipE",System,cellIndex++,mainMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," -1007 9 1 -2 -13 5 -6");
  makeCell("VoidS",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -9 -13 5 -6");
  makeCell("TipS",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1007 10 1 -2 14 5 -6");
  makeCell("VoidN",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -10 14 5 -6");
  makeCell("TipN",System,cellIndex++,mainMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," (-101:-103:-109) -3 -1 -1007 5 -6");
  makeCell("VoidSW",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," (-101:104:-107) 4 -1 -1007 5 -6");
  makeCell("VoidNW",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," (102:104:-108) 2 4 -1007 5 -6");
  makeCell("VoidNE",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," (102:-103:-110) -3 2 -1007 5 -6");
  makeCell("VoidSE",System,cellIndex++,0,0.0,Out);

  
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 4 107 101 -104 5 -6");
  makeCell("MainCellNW",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 4 108 -102 -104 5 -6");
  makeCell("MainCellNE",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -3 109 101 103 5 -6");
  makeCell("MainCellSW",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -3 110 -102 103 5 -6");
  makeCell("MainCellSE",System,cellIndex++,mainMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," -1007 5 -6");
  

  addOuterSurf(Out);

  return;
}


void
PIKFuelRod::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createLinks");

  FixedComp::setConnect(0,Origin-Y*(outerR/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(outerR/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(radius/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(radius/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
PIKFuelRod::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // pikSystem
