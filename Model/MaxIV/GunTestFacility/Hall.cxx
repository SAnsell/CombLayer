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
    gunRoomLength(A.gunRoomLength),gunRoomWidth(A.gunRoomWidth),
    klystronRoomWidth(A.klystronRoomWidth),
    height(A.height),
    depth(A.depth),
    backWallThick(A.backWallThick),
    gunRoomEntranceWidth(A.gunRoomEntranceWidth),
    midWallThick(A.midWallThick),
    outerWallThick(A.outerWallThick),
    klystronRoomWallThick(A.klystronRoomWallThick),
    mazeWidth(A.mazeWidth),
    mazeEntranceOffset(A.mazeEntranceOffset),
    mazeEntranceWidth(A.mazeEntranceWidth),
    hallLength(A.hallLength),
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
	gunRoomLength=A.gunRoomLength;
        gunRoomWidth=A.gunRoomWidth;
        klystronRoomWidth=A.klystronRoomWidth;
        height=A.height;
        depth=A.depth;
        backWallThick=A.backWallThick;
        gunRoomEntranceWidth=A.gunRoomEntranceWidth;
        midWallThick=A.midWallThick;
        outerWallThick=A.outerWallThick;
        klystronRoomWallThick=A.klystronRoomWallThick;
        mazeWidth=A.mazeWidth;
        mazeEntranceOffset=A.mazeEntranceOffset;
        mazeEntranceWidth=A.mazeEntranceWidth;
        hallLength=A.hallLength;
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

    gunRoomLength=Control.EvalVar<double>(keyName+"GunRoomLength");
    gunRoomWidth=Control.EvalVar<double>(keyName+"GunRoomWidth");
    klystronRoomWidth=Control.EvalVar<double>(keyName+"KlystronRoomWidth");
    height=Control.EvalVar<double>(keyName+"Height");
    depth=Control.EvalVar<double>(keyName+"Depth");
    backWallThick=Control.EvalVar<double>(keyName+"BackWallThick");
    gunRoomEntranceWidth=Control.EvalVar<double>(keyName+"GunRoomEntranceWidth");
    midWallThick=Control.EvalVar<double>(keyName+"MidWallThick");
    outerWallThick=Control.EvalVar<double>(keyName+"OuterWallThick");
    klystronRoomWallThick=Control.EvalVar<double>(keyName+"KlystronRoomWallThick");
    mazeWidth=Control.EvalVar<double>(keyName+"MazeWidth");
    mazeEntranceOffset=Control.EvalVar<double>(keyName+"MazeEntranceOffset");
    mazeEntranceWidth=Control.EvalVar<double>(keyName+"MazeEntranceWidth");
    hallLength=Control.EvalVar<double>(keyName+"HallLength");

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


    SurfMap::makePlane("back",SMap,buildIndex+1,Origin-Y*(gunRoomLength/2.0),Y);
    SurfMap::makePlane("front",SMap,buildIndex+2,Origin+Y*(gunRoomLength/2.0),Y);

    SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(gunRoomWidth/2.0),X);
    SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(gunRoomWidth/2.0),X);

    SurfMap::makePlane("bottom",SMap,buildIndex+5,Origin-Z*(depth),Z);
    SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(height),Z);

    ModelSupport::buildShiftedPlane(SMap,buildIndex+11,buildIndex+1,Y,-outerWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+12,buildIndex+2,Y,backWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+13,buildIndex+3,X,-midWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+14,buildIndex+4,X,-gunRoomEntranceWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+21,buildIndex+12,Y,-klystronRoomWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+23,buildIndex+13,X,-klystronRoomWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+24,buildIndex+4,X,outerWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+31,buildIndex+12,Y,mazeWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+33,buildIndex+23,X,klystronRoomWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+34,buildIndex+3,X,-outerWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+41,buildIndex+31,Y,outerWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+51,buildIndex+12,Y,hallLength);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+43,buildIndex+3,X,mazeEntranceOffset);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+44,buildIndex+43,X,mazeEntranceWidth);

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

    const HeadRule tb = ModelSupport::getHeadRule(SMap,buildIndex," 5 -6 ");

    HeadRule Out;
    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 ");
    makeCell("GunRoom",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 13 -3 ");
    makeCell("MidWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -12 2 13 -14 ");
    makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 -12 14 -4 ");
    makeCell("BackWallVoid",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -1 23 -4 ");
    makeCell("OuterWallRight",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -41 11 -24 4 ");
    makeCell("OuterWallBottom",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -21 1 -13 33 ");
    makeCell("KlystronRoom",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -12 21 -13 23 ");
    makeCell("KlystronRoomWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -21 1 -33 23 ");
    makeCell("KlystronRoomWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -31 12 3 -4 ");
    makeCell("Maze",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -41 31 -43 3 ");
    makeCell("MazeWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -41 31 -44 43 ");
    makeCell("MazeEntrance",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -41 31 -4 44 ");
    makeCell("MazeWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -41 12 -3 34 ");
    makeCell("MazeWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," -41 12 23 -34 ");
    makeCell("BeyondMaze",System,cellIndex++,voidMat,0.0,Out*tb);


    Out=ModelSupport::getHeadRule(SMap,buildIndex," -51 41 23 -24 ");
    makeCell("Hall",System,cellIndex++,voidMat,0.0,Out*tb);


    Out=ModelSupport::getHeadRule(SMap,buildIndex," -51 11 23 -24 ");
    addOuterSurf(Out*tb);

    return;
  }


  void
  Hall::createLinks()
  /*!
    Create all the linkes
  */
  {
    ELog::RegMethod RegA("Hall","createLinks");

    // TODO: Check and use names for the links below:

    // FixedComp::setConnect(0,Origin-Y*(gunRoomLength/2.0),-Y);
    // FixedComp::setNamedLinkSurf(0,"Back",SurfMap::getSignedSurf("#back"));

    // FixedComp::setConnect(1,Origin+Y*(gunRoomLength/2.0),Y);
    // FixedComp::setNamedLinkSurf(1,"Front",SMap.realSurf(buildIndex+2));

    // FixedComp::setConnect(2,Origin-X*(gunRoomWidth/2.0),-X);
    // FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+3));

    // FixedComp::setConnect(3,Origin+X*(gunRoomWidth/2.0),X);
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
