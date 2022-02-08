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
  shellthick(A.shellthick),
  mainMat(A.mainMat),
  shellMat(A.shellMat),
  coolMat(A.coolMat)
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
      shellthick=A.shellthick;
      mainMat=A.mainMat;
      shellMat=A.shellMat;
      coolMat=A.coolMat;
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
  shellthick=Control.EvalVar<double>(keyName+"ShellThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  shellMat=ModelSupport::EvalMat<int>(Control,keyName+"ShellMat");
  coolMat=ModelSupport::EvalMat<int>(Control,keyName+"CoolMat");

  return;
}

void
PIKFuelRod::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PIKFuelRod","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(outerR-radius),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(outerR-radius),Y);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(outerR-radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(outerR-radius),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  double thickenCenter = radius+thickenR;
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(thickenCenter),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(thickenCenter),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(thickenCenter),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(thickenCenter),X);

  double _radius_, _outerR_, _thickenR_;
  int _index_;

  for(int i=0; i<2; i++) {

    _radius_=radius+shellthick*static_cast<double>(i);
    _outerR_=outerR+shellthick*static_cast<double>(i);
    _thickenR_=thickenR-shellthick*static_cast<double>(i);
    _index_=buildIndex+i*100;

    ModelSupport::buildCylinder(SMap,_index_+1007,Origin,Z,_outerR_);

    ModelSupport::buildPlane(SMap,_index_+1,Origin-Y*(_radius_),Y);
    ModelSupport::buildPlane(SMap,_index_+2,Origin+Y*(_radius_),Y);

    ModelSupport::buildPlane(SMap,_index_+3,Origin-X*(_radius_),X);
    ModelSupport::buildPlane(SMap,_index_+4,Origin+X*(_radius_),X);

    ModelSupport::buildCylinder(SMap,_index_+7,Origin-Y*(outerR-radius),Z,_radius_);
    ModelSupport::buildCylinder(SMap,_index_+8,Origin+Y*(outerR-radius),Z,_radius_);
    ModelSupport::buildCylinder(SMap,_index_+9,Origin-X*(outerR-radius),Z,_radius_);
    ModelSupport::buildCylinder(SMap,_index_+10,Origin+X*(outerR-radius),Z,_radius_);

    ModelSupport::buildCylinder(SMap,_index_+27,Origin-Y*(thickenCenter)+X*(thickenCenter),Z,_thickenR_);
    ModelSupport::buildCylinder(SMap,_index_+28,Origin+Y*(thickenCenter)+X*(thickenCenter),Z,_thickenR_);
    ModelSupport::buildCylinder(SMap,_index_+29,Origin-Y*(thickenCenter)-X*(thickenCenter),Z,_thickenR_);
    ModelSupport::buildCylinder(SMap,_index_+30,Origin+Y*(thickenCenter)-X*(thickenCenter),Z,_thickenR_);

  };

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

  HeadRule HR;

  //  fuel rod up-bottom bounds
  HeadRule HRub=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6");

  /*
   Uranium part
  */

  // FR cross
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 3 -4");
  makeCell("MainCell1",System,cellIndex++,mainMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -3");
  makeCell("MainCell2",System,cellIndex++,mainMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 4 -14");
  makeCell("MainCell3",System,cellIndex++,mainMat,0.0,HR*HRub);


  // FR thickening in the center
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 4 27 21 -24");
  makeCell("MainCellNW",System,cellIndex++,mainMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 4 28 -22 -24");
  makeCell("MainCellNE",System,cellIndex++,mainMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -3 29 21 23");
  makeCell("MainCellSW",System,cellIndex++,mainMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -3 30 -22 23");
  makeCell("MainCellSE",System,cellIndex++,mainMat,0.0,HR*HRub);

  // FR tips
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -11");
  makeCell("TipW",System,cellIndex++,mainMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-8 12");
  makeCell("TipE",System,cellIndex++,mainMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-9 -13");
  makeCell("TipS",System,cellIndex++,mainMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-10 14");
  makeCell("TipN",System,cellIndex++,mainMat,0.0,HR*HRub);

  /*
   Shell part
  */

  // shell tips
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"7 -107 -11");
  makeCell("ShellTipW",System,cellIndex++,shellMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"8 -108 12");
  makeCell("ShellTipE",System,cellIndex++,shellMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"9 -109 -13");
  makeCell("ShellTipS",System,cellIndex++,shellMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"10 -110 14");
  makeCell("ShellTipN",System,cellIndex++,shellMat,0.0,HR*HRub);


  // middle shell tips
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(4:-3) -104 103")*HRub;

  makeCell("ShellW",System,cellIndex++,shellMat,0.0,
    HR*ModelSupport::getHeadRule(SMap,buildIndex,"11 -21"));

  makeCell("ShellE",System,cellIndex++,shellMat,0.0,
    HR*ModelSupport::getHeadRule(SMap,buildIndex,"22 -12"));


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-1:2) 101 -102")*HRub;

  makeCell("ShellN",System,cellIndex++,shellMat,0.0,
    HR*ModelSupport::getHeadRule(SMap,buildIndex,"24 -14"));

  makeCell("ShellS",System,cellIndex++,shellMat,0.0,
    HR*ModelSupport::getHeadRule(SMap,buildIndex,"13 -23"));


  // shell thickening in the center
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"127 -27 21 -24");
  makeCell("ShellNW",System,cellIndex++,shellMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"128 -28 -22 -24");
  makeCell("ShellNE",System,cellIndex++,shellMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"129 -29 21 23");
  makeCell("ShellSW",System,cellIndex++,shellMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"130 -30 -22 23");
  makeCell("ShellSE",System,cellIndex++,shellMat,0.0,HR*HRub);

  // light-water

  // water near tips
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1107 107 103 -104 -11");
  makeCell("VoidW",System,cellIndex++,coolMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1107 108 103 -104 12");
  makeCell("VoidE",System,cellIndex++,coolMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1107 109 101 -102 -13");
  makeCell("VoidS",System,cellIndex++,coolMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1107 110 101 -102 14");
  makeCell("VoidN",System,cellIndex++,coolMat,0.0,HR*HRub);

  // water in the corners
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-21:-23:-129) -103 -101 -1107");
  makeCell("VoidSW",System,cellIndex++,coolMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-21:24:-127) 104 -101 -1107");
  makeCell("VoidNW",System,cellIndex++,coolMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(22:24:-128) 102 104 -1107");
  makeCell("VoidNE",System,cellIndex++,coolMat,0.0,HR*HRub);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(22:-23:-130) -103 102 -1107");
  makeCell("VoidSE",System,cellIndex++,coolMat,0.0,HR*HRub);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1107")*HRub;

  addOuterSurf(HR);

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
