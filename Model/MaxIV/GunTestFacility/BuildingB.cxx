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
#include "objectRegister.h"

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
#include "Duct.h"

#include "BuildingB.h"

namespace MAXIV::GunTestFacility
{

  BuildingB::BuildingB(const std::string& Key)  :
    attachSystem::ContainedComp(),
    attachSystem::FixedRotate(Key,12),
    attachSystem::CellMap(),
    attachSystem::SurfMap(),
    ductWave(std::make_shared<xraySystem::Duct>(keyName+"DuctWave")),
    ductSignal1(std::make_shared<xraySystem::Duct>(keyName+"DuctSignal1")),
    ductSignal2(std::make_shared<xraySystem::Duct>(keyName+"DuctSignal2")),
    ductSignal3(std::make_shared<xraySystem::Duct>(keyName+"DuctSignal3")),
    ductWater1(std::make_shared<xraySystem::Duct>(keyName+"DuctWater1")),
    ductVent(std::make_shared<xraySystem::Duct>(keyName+"DuctVentillationPenetration")),
    ductLaser(std::make_shared<xraySystem::Duct>(keyName+"DuctLaser")),
    ductSignal4(std::make_shared<xraySystem::Duct>(keyName+"DuctSignal4")),
    ductWater2(std::make_shared<xraySystem::Duct>(keyName+"DuctWater2")),
    ductSuction(std::make_shared<xraySystem::Duct>(keyName+"DuctSuctionFan")),
    ductVentRoof1(std::make_shared<xraySystem::Duct>(keyName+"DuctVentillationRoof1")),
    ductVentRoof2(std::make_shared<xraySystem::Duct>(keyName+"DuctVentillationRoof2"))
    /*!
      Constructor BUT ALL variable are left unpopulated.
      \param Key :: Name for item in search
    */
  {
    ModelSupport::objectRegister& OR=
      ModelSupport::objectRegister::Instance();

    OR.addObject(ductWave);
    OR.addObject(ductSignal1);
    OR.addObject(ductSignal2);
    OR.addObject(ductSignal3);
    OR.addObject(ductWater1);
    OR.addObject(ductVent);
    OR.addObject(ductLaser);
    OR.addObject(ductSignal4);
    OR.addObject(ductWater2);
    OR.addObject(ductSuction);
    OR.addObject(ductVentRoof1);
    OR.addObject(ductVentRoof2);
  }

  BuildingB::BuildingB(const BuildingB& A) :
    attachSystem::ContainedComp(A),
    attachSystem::FixedRotate(A),
    attachSystem::CellMap(A),
    attachSystem::SurfMap(A),
    gunRoomLength(A.gunRoomLength),gunRoomWidth(A.gunRoomWidth),
    klystronRoomWidth(A.klystronRoomWidth),
    height(A.height),
    hallHeight(A.hallHeight),
    depth(A.depth),
    eastClearance(A.eastClearance),
    backWallThick(A.backWallThick),
    gunRoomEntranceWidth(A.gunRoomEntranceWidth),
    midWallThick(A.midWallThick),
    outerWallThick(A.outerWallThick),
    internalWallThick(A.internalWallThick),
    mazeWidth(A.mazeWidth),
    mazeEntranceOffset(A.mazeEntranceOffset),
    mazeEntranceWidth(A.mazeEntranceWidth),
    mazeEntranceHeight(A.mazeEntranceHeight),
    hallLength(A.hallLength),
    floorThick(A.floorThick),
    roofGunTestThick(A.roofGunTestThick),
    trspRoomWidth(A.trspRoomWidth),
    stairRoomWidth(A.stairRoomWidth),
    stairRoomLength(A.stairRoomLength),
    elevatorWidth(A.elevatorWidth),
    elevatorLength(A.elevatorLength),
    oilRoomEntranceWidth(A.oilRoomEntranceWidth),
    oilRoomWallThick(A.oilRoomWallThick),
    controlRoomWidth(A.controlRoomWidth),
    controlRoomLength(A.controlRoomLength),
    controlRoomWallThick(A.controlRoomWallThick),
    controlRoomEntranceWidth(A.controlRoomEntranceWidth),
    controlRoomEntranceOffset(A.controlRoomEntranceOffset),
    level9Height(A.level9Height),
    level9RoofThick(A.level9RoofThick),
    wallMat(A.wallMat),
    voidMat(A.voidMat),
    oilRoomWallMat(A.oilRoomWallMat)
    /*!
      Copy constructor
      \param A :: BuildingB to copy
    */
  {}

  BuildingB&
  BuildingB::operator=(const BuildingB& A)
  /*!
    Assignment operator
    \param A :: BuildingB to copy
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
        hallHeight=A.hallHeight;
        depth=A.depth;
        eastClearance=A.eastClearance;
        backWallThick=A.backWallThick;
        gunRoomEntranceWidth=A.gunRoomEntranceWidth;
        midWallThick=A.midWallThick;
        outerWallThick=A.outerWallThick;
        internalWallThick=A.internalWallThick;
        mazeWidth=A.mazeWidth;
        mazeEntranceOffset=A.mazeEntranceOffset;
        mazeEntranceWidth=A.mazeEntranceWidth;
        mazeEntranceHeight=A.mazeEntranceHeight;
        hallLength=A.hallLength;
        floorThick=A.floorThick;
        roofGunTestThick=A.roofGunTestThick;
        trspRoomWidth=A.trspRoomWidth;
        stairRoomWidth=A.stairRoomWidth;
        stairRoomLength=A.stairRoomLength;
        elevatorWidth=A.elevatorWidth;
        elevatorLength=A.elevatorLength;
        oilRoomEntranceWidth=A.oilRoomEntranceWidth;
        oilRoomWallThick=A.oilRoomWallThick;
        controlRoomWidth=A.controlRoomWidth;
        controlRoomLength=A.controlRoomLength;
        controlRoomWallThick=A.controlRoomWallThick;
        controlRoomEntranceWidth=A.controlRoomEntranceWidth;
        controlRoomEntranceOffset=A.controlRoomEntranceOffset;
        level9Height=A.level9Height;
        level9RoofThick=A.level9RoofThick;
	wallMat=A.wallMat;
	voidMat=A.voidMat;
        oilRoomWallMat=A.oilRoomWallMat;
      }
    return *this;
  }

  BuildingB*
  BuildingB::clone() const
  /*!
    Clone self
    \return new (this)
  */
  {
    return new BuildingB(*this);
  }

  BuildingB::~BuildingB()
  /*!
    Destructor
  */
  {}

  void
  BuildingB::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
  {
    ELog::RegMethod RegA("BuildingB","populate");

    FixedRotate::populate(Control);

    gunRoomLength=Control.EvalVar<double>(keyName+"GunRoomLength");
    gunRoomWidth=Control.EvalVar<double>(keyName+"GunRoomWidth");
    klystronRoomWidth=Control.EvalVar<double>(keyName+"KlystronRoomWidth");
    height=Control.EvalVar<double>(keyName+"Height");
    hallHeight=Control.EvalVar<double>(keyName+"HallHeight");
    depth=Control.EvalVar<double>(keyName+"Depth");
    eastClearance=Control.EvalVar<double>(keyName+"EastClearance");
    backWallThick=Control.EvalVar<double>(keyName+"BackWallThick");
    gunRoomEntranceWidth=Control.EvalVar<double>(keyName+"GunRoomEntranceWidth");
    midWallThick=Control.EvalVar<double>(keyName+"MidWallThick");
    outerWallThick=Control.EvalVar<double>(keyName+"OuterWallThick");
    internalWallThick=Control.EvalVar<double>(keyName+"InternalWallThick");
    mazeWidth=Control.EvalVar<double>(keyName+"MazeWidth");
    mazeEntranceOffset=Control.EvalVar<double>(keyName+"MazeEntranceOffset");
    mazeEntranceWidth=Control.EvalVar<double>(keyName+"MazeEntranceWidth");
    mazeEntranceHeight=Control.EvalVar<double>(keyName+"MazeEntranceHeight");
    hallLength=Control.EvalVar<double>(keyName+"HallLength");
    floorThick=Control.EvalVar<double>(keyName+"FloorThick");
    roofGunTestThick=Control.EvalVar<double>(keyName+"RoofGunTestThick");
    trspRoomWidth=Control.EvalVar<double>(keyName+"TRSPRoomWidth");
    stairRoomWidth=Control.EvalVar<double>(keyName+"StairRoomWidth");
    stairRoomLength=Control.EvalVar<double>(keyName+"StairRoomLength");
    elevatorWidth=Control.EvalVar<double>(keyName+"ElevatorWidth");
    elevatorLength=Control.EvalVar<double>(keyName+"ElevatorLength");
    oilRoomEntranceWidth=Control.EvalVar<double>(keyName+"OilRoomEntranceWidth");
    oilRoomWallThick=Control.EvalVar<double>(keyName+"OilRoomWallThick");
    controlRoomWidth=Control.EvalVar<double>(keyName+"ControlRoomWidth");
    controlRoomLength=Control.EvalVar<double>(keyName+"ControlRoomLength");
    controlRoomWallThick=Control.EvalVar<double>(keyName+"ControlRoomWallThick");
    controlRoomEntranceWidth=Control.EvalVar<double>(keyName+"ControlRoomEntranceWidth");
    controlRoomEntranceOffset=Control.EvalVar<double>(keyName+"ControlRoomEntranceOffset");
    level9Height=Control.EvalVar<double>(keyName+"Level9Height");
    level9RoofThick=Control.EvalVar<double>(keyName+"Level9RoofThick");

    wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
    voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat","Void");
    oilRoomWallMat=ModelSupport::EvalMat<int>(Control,keyName+"OilRoomWallMat");

    if (std::abs(gunRoomWidth + midWallThick+ klystronRoomWidth + trspRoomWidth + stairRoomWidth + 2.0*internalWallThick-2025.0)>1e-3)
      ELog::EM << "WARNING: gun test room widths do not sum up to 2025 as of K_20-1_08C6b4" << ELog::endWarn;

    return;
  }

  void
  BuildingB::createSurfaces()
  /*!
    Create All the surfaces
  */
  {
    ELog::RegMethod RegA("BuildingB","createSurfaces");


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
    ModelSupport::buildShiftedPlane(SMap,buildIndex+15,buildIndex+5,Y,-floorThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+16,buildIndex+6,Y,roofGunTestThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+21,buildIndex+12,Y,-internalWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+23,buildIndex+13,X,-klystronRoomWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+24,buildIndex+4,X,outerWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+25,buildIndex+5,Y,mazeEntranceHeight);
    ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(hallHeight),Z);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+31,buildIndex+12,Y,mazeWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+32,buildIndex+31,Y,outerWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+33,buildIndex+23,X,-internalWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+34,buildIndex+3,X,-outerWallThick);

    ModelSupport::buildShiftedPlane(SMap,buildIndex+35,buildIndex+16,Z,level9Height);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+36,buildIndex+35,Z,level9RoofThick);

    ModelSupport::buildShiftedPlane(SMap,buildIndex+41,buildIndex+12,Y,hallLength);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+42,buildIndex+41,Y,internalWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+43,buildIndex+3,X,mazeEntranceOffset);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+44,buildIndex+43,X,mazeEntranceWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+51,buildIndex+1,X,stairRoomLength);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+52,buildIndex+51,X,internalWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+53,buildIndex+33,X,-trspRoomWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+61,buildIndex+51,X,
				    -elevatorLength-internalWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+62,buildIndex+51,X,-elevatorLength);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+63,buildIndex+53,X,-internalWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+71,buildIndex+12,X,-oilRoomWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+73,buildIndex+63,X,-stairRoomWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+74,buildIndex+73,X,elevatorWidth);

    ModelSupport::buildShiftedPlane(SMap,buildIndex+81,buildIndex+42,Y,eastClearance);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+82,buildIndex+81,Y,outerWallThick);

    ModelSupport::buildShiftedPlane(SMap,buildIndex+83,buildIndex+73,X,-outerWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+84,buildIndex+74,X,internalWallThick);

    ModelSupport::buildShiftedPlane(SMap,buildIndex+91,buildIndex+82,Y,controlRoomLength);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+92,buildIndex+91,Y,internalWallThick);

    ModelSupport::buildShiftedPlane(SMap,buildIndex+93,buildIndex+63,Y,
				    -oilRoomEntranceWidth-oilRoomWallThick);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+94,buildIndex+93,Y,oilRoomWallThick);


    ModelSupport::buildShiftedPlane(SMap,buildIndex+103,buildIndex+73,X,controlRoomWidth);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+104,buildIndex+103,X,internalWallThick); // TODO intentionally wrong thickness. Need to be constructed in a separate class for B1. See [3].

    ModelSupport::buildShiftedPlane(SMap,buildIndex+203,buildIndex+73,X,controlRoomEntranceOffset);
    ModelSupport::buildShiftedPlane(SMap,buildIndex+204,buildIndex+203,X,controlRoomEntranceWidth);

    return;
  }

  void
  BuildingB::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
  {
    ELog::RegMethod RegA("BuildingB","createObjects");

    const HeadRule tb = ModelSupport::getHeadRule(SMap,buildIndex," 5 -26 ");

    HeadRule Out;
    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 5 -6");
    makeCell("GunRoom",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 13 -3 ");
    makeCell("MidWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 -12 13 -14 ");
    makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 -12 14 -4 5 -6");
    makeCell("GunRoomEntrance",System,cellIndex++,voidMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 -12 14 -4 6 -26 ");
    makeCell("GunRoomEntranceLintel",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -1 83 -4 5 -36");
    makeCell("OuterWallEast",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -42 4 -24 5 -36");
    makeCell("OuterWallNorth",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -21 23 -13 ");
    makeCell("KlystronRoom",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 21 -12 33 -13 ");
    makeCell("KlystronRoomWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -21 33 -23 ");
    makeCell("KlystronRoomWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 12 -31 3 -4 ");
    makeCell("Maze",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 31 -32 3 -43 5 -25");
    makeCell("MazeWallJamb",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 31 -32 43 -44 5 -25");
    makeCell("MazeEntrance",System,cellIndex++,voidMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 31 -32 3 -44 25 -26");
    makeCell("MazeEntranceLintel",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 31 -32 44 -4 ");
    makeCell("MazeWallSide",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 12 -32 34 -3 ");
    makeCell("MazeWallFront",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 12 -41 73 -34 ");
    makeCell("BeyondMaze",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 32 -41 34 -4 ");
    makeCell("Hall",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 41 -42 83 -203 ");
    makeCell("EastWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 41 -82 203 -204 ");
    makeCell("ControlRoomEntrance",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 41 -42 204 -4 ");
    makeCell("EastWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -12 53 -33 5 -16");
    makeCell("CraneRoom",System,cellIndex++,voidMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -12 63 -53 ");
    makeCell("CraneRoomSouthWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -61 73 -63 ");
    makeCell("StairRoom",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 61 -51 84 -63 ");
    makeCell("StairRoom",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 51 -52 73 -63 ");
    makeCell("StairRoomEastWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 52 -71 73 -93 ");
    makeCell("OilRoom",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 71 -12 73 -94 ");
    makeCell("OilRoomEastWall",System,cellIndex++,oilRoomWallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 52 -71 93 -94 ");
    makeCell("OilRoomNorthWall",System,cellIndex++,oilRoomWallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 52 -12 94 -63 ");
    makeCell("OilRoomAnteroom",System,cellIndex++,oilRoomWallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 62 -51 73 -74 ");
    makeCell("Elevator",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 61 -62 73 -74 ");
    makeCell("ElevatorLeftWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 61 -51 74 -84 ");
    makeCell("ElevatorTopWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -41 83 -73 5 -36");
    makeCell("OuterWallSouth",System,cellIndex++,wallMat,0.0,Out);

    // Actually, control room belongs to Bulding B1, not Building B,
    // but for now let's construct it within Building B making all the cells
    // indeptndent from the "native" Building B cells

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 42 -81 83 -203 ");
    makeCell("EastClearance",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 42 -81 204 -24 ");
    makeCell("EastClearance",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 81 -82 73 -203 ");
    makeCell("B1WestWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 81 -82 204 -24 ");
    makeCell("B1WestWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 82 -91 73 -103 ");
    makeCell("ControlRoom",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 82 -91 103 -104 ");
    makeCell("ControlRoomNorthWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 82 -92 104 -24 ");
    makeCell("B1Void",System,cellIndex++,voidMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 81 -92 83 -73 ");
    makeCell("ControlRoomSouthWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 91 -92 73 -104 ");
    makeCell("ControlRoomEastWall",System,cellIndex++,wallMat,0.0,Out*tb);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 42 -92 4 -24 26 -16");
    makeCell("ControlRoomWallCell",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -92 83 -24 15 -5");
    makeCell("Floor",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 6 -16");
    makeCell("Roof8GunTest",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 2 -92 3 -4 26 -16");
    makeCell("Roof8Other",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -41 73 -3 26 -16 (12:-53:33) ");
    makeCell("Roof8Other",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 41 -92 83 -3 26 -16 ");
    makeCell("Roof8B1",System,cellIndex++,wallMat,0.0,Out);

    // Level 9
    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -41 73 -4 16 -35");
    makeCell("Level9",System,cellIndex++,voidMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 1 -41 73 -4 35 -36");
    makeCell("Level9Roof",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 41 -42 83 -4 16 -36");
    makeCell("Level9EastWall",System,cellIndex++,wallMat,0.0,Out);

    Out=ModelSupport::getHeadRule(SMap,buildIndex," 42 -92 83 -24 16 -36");
    makeCell("Level9B1Void",System,cellIndex++,voidMat,0.0,Out);


    Out=ModelSupport::getHeadRule(SMap,buildIndex," 11 -92 83 -24 15 -36");
    addOuterSurf(Out);

    return;
  }


  void
  BuildingB::createLinks()
  /*!
    Create all the links
  */
  {
    ELog::RegMethod RegA("BuildingB","createLinks");

    // TODO: Check and use names for the links below:

    FixedComp::setConnect(0,Origin-Y*(gunRoomLength/2.0+outerWallThick),-Y);
    FixedComp::setNamedLinkSurf(0,"Back",-SMap.realSurf(buildIndex+11));

    FixedComp::setConnect(1,Origin+Y*(gunRoomLength/2.0+backWallThick+hallLength+internalWallThick),Y);
    FixedComp::setNamedLinkSurf(1,"Front",SMap.realSurf(buildIndex+42));

    FixedComp::setConnect(2,Origin-X*(gunRoomWidth/2.0+midWallThick+klystronRoomWidth+
				      2*internalWallThick+trspRoomWidth+stairRoomWidth+
				      outerWallThick),-X);
    FixedComp::setNamedLinkSurf(2,"Left",-SMap.realSurf(buildIndex+83));

    FixedComp::setConnect(3,Origin+X*(gunRoomWidth/2.0+outerWallThick),X);
    FixedComp::setNamedLinkSurf(3,"Right",SMap.realSurf(buildIndex+24));

    FixedComp::setConnect(4,Origin-Z*(depth+floorThick),-Z);
    FixedComp::setNamedLinkSurf(4,"Bottom",-SMap.realSurf(buildIndex+15));

    FixedComp::setConnect(5,Origin+Z*(height+roofGunTestThick),Z);
    FixedComp::setNamedLinkSurf(5,"RoofGunTestTop",SMap.realSurf(buildIndex+16));


    FixedComp::setConnect(6,Origin-X*(gunRoomWidth/2.0+midWallThick),X);
    FixedComp::setNamedLinkSurf(6,"MidWallBack",SMap.realSurf(buildIndex+13));

    FixedComp::setConnect(7,Origin-X*(gunRoomWidth/2.0),-X);
    FixedComp::setNamedLinkSurf(7,"MidWallFront",-SMap.realSurf(buildIndex+3));

    FixedComp::setConnect(8,Origin-X*(gunRoomWidth/2.0+outerWallThick),X);
    FixedComp::setNamedLinkSurf(8,"MazeWallFrontBack",SMap.realSurf(buildIndex+34));

    FixedComp::setConnect(9,Origin+Y*(gunRoomLength/2.0+backWallThick+mazeWidth),Y);
    FixedComp::setNamedLinkSurf(9,"MazeWallSideBack",SMap.realSurf(buildIndex+31));

    FixedComp::setConnect(10,Origin+Y*(gunRoomLength/2.0+backWallThick+mazeWidth+outerWallThick),Y);
    FixedComp::setNamedLinkSurf(10,"MazeWallSideFront",SMap.realSurf(buildIndex+32));

    FixedComp::setConnect(11,Origin+Z*(height),Z);
    FixedComp::setNamedLinkSurf(11,"RoofGunTestBottom",SMap.realSurf(buildIndex+6));

    return;
  }

  void
  BuildingB::createDucts(Simulation& System)
  /*!
    Create all the ducts
  */
  {
    ELog::RegMethod RegA("BuildingB","createDucts");

    ductWave->setFront(getFullRule("MidWallFront"));
    ductWave->setBack(getFullRule("MidWallBack"));
    ductWave->addInsertCell(getCell("MidWall"));
    ductWave->createAll(System,*this,0);

    ductSignal1->setFront(getFullRule("MidWallFront"));
    ductSignal1->setBack(getFullRule("MidWallBack"));
    ductSignal1->addInsertCell(getCell("MidWall"));
    ductSignal1->createAll(System,*this,0);

    ductSignal2->setFront(getFullRule("MidWallFront"));
    ductSignal2->setBack(getFullRule("MidWallBack"));
    ductSignal2->addInsertCell(getCell("MidWall"));
    ductSignal2->createAll(System,*this,0);

    ductSignal3->setFront(getFullRule("MidWallFront"));
    ductSignal3->setBack(getFullRule("MidWallBack"));
    ductSignal3->addInsertCell(getCell("MidWall"));
    ductSignal3->createAll(System,*this,0);

    ductWater1->setFront(getFullRule("MidWallFront"));
    ductWater1->setBack(getFullRule("MidWallBack"));
    ductWater1->addInsertCell(getCell("MidWall"));
    ductWater1->createAll(System,*this,0);

    ductVent->setFront(getFullRule("MidWallFront"));
    ductVent->setBack(getFullRule("MazeWallFrontBack"));
    ductVent->addInsertCell(getCell("MazeWallFront"));
    ductVent->createAll(System,*this,0);

    ductLaser->setFront(getFullRule("#MazeWallSideFront"));
    ductLaser->setBack(getFullRule("MazeWallSideBack"));
    ductLaser->addInsertCell(getCell("MazeWallSide"));
    ductLaser->createAll(System,*this,0);

    ductSignal4->setFront(getFullRule("#MazeWallSideFront"));
    ductSignal4->setBack(getFullRule("MazeWallSideBack"));
    ductSignal4->addInsertCell(getCell("MazeWallSide"));
    ductSignal4->createAll(System,*this,0);

    ductWater2->setFront(getFullRule("#MazeWallSideFront"));
    ductWater2->setBack(getFullRule("MazeWallSideBack"));
    ductWater2->addInsertCell(getCell("MazeEntranceLintel"));
    ductWater2->createAll(System,*this,0);

    ductSuction->setFront(getFullRule("#MazeWallSideFront"));
    ductSuction->setBack(getFullRule("MazeWallSideBack"));
    ductSuction->addInsertCell(getCell("MazeEntranceLintel"));
    ductSuction->createAll(System,*this,0);

    ductVentRoof1->setFront(getFullRule("RoofGunTestBottom"));
    ductVentRoof1->setBack(getFullRule("#RoofGunTestTop"));
    ductVentRoof1->addInsertCell(getCell("Roof8GunTest"));
    ductVentRoof1->createAll(System,*this,"RoofGunTestBottom");

    ductVentRoof2->setFront(getFullRule("RoofGunTestBottom"));
    ductVentRoof2->setBack(getFullRule("#RoofGunTestTop"));
    ductVentRoof2->addInsertCell(getCell("Roof8GunTest"));
    ductVentRoof2->createAll(System,*this,"RoofGunTestBottom");
  }

  void
  BuildingB::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
  {
    ELog::RegMethod RegA("BuildingB","createAll");

    populate(System.getDataBase());
    createUnitVector(FC,sideIndex);
    createSurfaces();
    createObjects(System);
    createLinks();

    createDucts(System);

    insertObjects(System);

    return;
  }

}
