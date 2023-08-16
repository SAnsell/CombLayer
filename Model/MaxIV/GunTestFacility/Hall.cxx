/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/GunTestFacility/Hall.cxx
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
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"

#include "Hall.h"

namespace MAXIV::GunTestFacility
{

  Hall::Hall(const std::string& Key)  :
    attachSystem::ContainedComp(),
    attachSystem::FixedRotate(Key,6),
    attachSystem::CellMap(),
    attachSystem::SurfMap()
    /*!
      Constructor BUT ALL variable are left unpopulated.
      \param Key :: Name for item in search
    */
  {}

  Hall::Hall(const Hall& A) :
    attachSystem::ContainedComp(A),
    attachSystem::FixedRotate(A),
    attachSystem::CellMap(A),
    attachSystem::SurfMap(A),
    mainRoomLength(A.mainRoomLength),mainRoomWidth(A.mainRoomWidth),height(A.height),
    labRoomWidth(A.labRoomWidth),
    depth(A.depth),
    backWallThick(A.backWallThick),
    backWallLength(A.backWallLength),
    midWallThick(A.midWallThick),
    outerWallThick(A.outerWallThick),
    labRoomWallThick(A.labRoomWallThick),
    wallMat(A.wallMat),
    voidMat(A.voidMat)
    /*!
      Copy constructor
      \param A :: Hall to copy
    */
  {}

  Hall&
  Hall::operator=(const Hall& A)
  /*!
    Assignment operator
    \param A :: Hall to copy
    \return *this
  */
  {
    if (this!=&A)
      {
	attachSystem::ContainedComp::operator=(A);
	attachSystem::FixedRotate::operator=(A);
	attachSystem::CellMap::operator=(A);
	mainRoomLength=A.mainRoomLength;
        mainRoomWidth=A.mainRoomWidth;
        labRoomWidth=A.labRoomWidth;
        height=A.height;
        depth=A.depth;
        backWallThick=A.backWallThick;
        backWallLength=A.backWallLength;
        midWallThick=A.midWallThick;
        outerWallThick=A.outerWallThick;
        labRoomWallThick=A.labRoomWallThick;
	wallMat=A.wallMat;
	voidMat=A.voidMat;
      }
    return *this;
  }

  Hall*
  Hall::clone() const
  /*!
    Clone self
    \return new (this)
  */
  {
    return new Hall(*this);
  }

  Hall::~Hall()
  /*!
    Destructor
  */
  {}

  void
  Hall::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
  {
    ELog::RegMethod RegA("Hall","populate");

    FixedRotate::populate(Control);

    mainRoomLength=Control.EvalVar<double>(keyName+"MainRoomLength");
    mainRoomWidth=Control.EvalVar<double>(keyName+"MainRoomWidth");
    labRoomWidth=Control.EvalVar<double>(keyName+"LabRoomWidth");
    height=Control.EvalVar<double>(keyName+"Height");
    depth=Control.EvalVar<double>(keyName+"Depth");
    backWallThick=Control.EvalVar<double>(keyName+"BackWallThick");
    backWallLength=Control.EvalVar<double>(keyName+"BackWallLength");
    midWallThick=Control.EvalVar<double>(keyName+"MidWallThick");
    outerWallThick=Control.EvalVar<double>(keyName+"OuterWallThick");
    labRoomWallThick=Control.EvalVar<double>(keyName+"LabRoomWallThick");

    wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
    voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat","Void");

    return;
  }

  void
  Hall::createSurfaces()
  /*!
    Create All the surfaces
  */
  {
    ELog::RegMethod RegA("Hall","createSurfaces");


    SurfMap::makePlane("back",SMap,buildIndex+1,Origin-Y*(mainRoomLength/2.0),Y);
    SurfMap::makePlane("front",SMap,buildIndex+2,Origin+Y*(mainRoomLength/2.0),Y);

    SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(mainRoomWidth/2.0),X);
    SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(mainRoomWidth/2.0),X);

    SurfMap::makePlane("bottom",SMap,buildIndex+5,Origin-Z*(depth),Z);
    SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(height),Z);

    ModelSupport::buildShiftedPlane(SMap,buildIndex+11,buildIndex+1,Y,-backWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+12,buildIndex+2,Y,outerWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+13,buildIndex+4,Y,-backWallLength);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+14,buildIndex+4,Y,midWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+21,buildIndex+11,Y,labRoomWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+24,buildIndex+14,Y,labRoomWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+34,buildIndex+24,Y,-labRoomWallThick);

    return;
  }

  void
  Hall::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
  {
    ELog::RegMethod RegA("Hall","createObjects");

    HeadRule Out;
    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
    makeCell("MainRoom",System,cellIndex++,voidMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 4 -14 5 -6 ");
    makeCell("MidWall",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -1 13 -14 5 -6 ");
    makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -1 3 -13 5 -6 ");
    makeCell("BackWallVoid",System,cellIndex++,voidMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 -12 3 -24 5 -6 ");
    makeCell("OuterWallRight",System,cellIndex++,wallMat,0.0,Out);


    Out=ModelSupport::getHeadRule(SMap,buildIndex," 21 -2 14 -34 5 -6 ");
    makeCell("LabRoom",System,cellIndex++,voidMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -21 14 -24 5 -6 ");
    makeCell("LabRoomWall",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 21 -2 34 -24 5 -6 ");
    makeCell("LabRoomWall",System,cellIndex++,wallMat,0.0,Out);


    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -12 3 -24 5 -6 ");
    addOuterSurf(Out);

    return;
  }


  void
  Hall::createLinks()
  /*!
    Create all the linkes
  */
  {
    ELog::RegMethod RegA("Hall","createLinks");

    FixedComp::setConnect(0,Origin-Y*(mainRoomLength/2.0),-Y);
    FixedComp::setNamedLinkSurf(0,"Back",SurfMap::getSignedSurf("#back"));

    // TODO: Check and use names for the links below:

    // FixedComp::setConnect(1,Origin+Y*(mainRoomLength/2.0),Y);
    // FixedComp::setNamedLinkSurf(1,"Front",SMap.realSurf(buildIndex+2));

    // FixedComp::setConnect(2,Origin-X*(mainRoomWidth/2.0),-X);
    // FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

    // FixedComp::setConnect(3,Origin+X*(mainRoomWidth/2.0),X);
    // FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+4));

    // FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
    // FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+5));

    // FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
    // FixedComp::setNamedLinkSurf(5,"Top",SMap.realSurf(buildIndex+6));

    return;
  }

  void
  Hall::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
  {
    ELog::RegMethod RegA("Hall","createAll");

    populate(System.getDataBase());
    createUnitVector(FC,sideIndex);
    createSurfaces();
    createObjects(System);
    createLinks();
    insertObjects(System);

    return;
  }

}
