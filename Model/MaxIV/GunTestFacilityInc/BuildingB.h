/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/GunTestFacility/Hall.h
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
#ifndef xraySystem_GunTestFacilityBuildingB_h
#define xraySystem_GunTestFacilityBuildingB_h

class Simulation;
class ConcreteDoor;

namespace xraySystem {
  class Duct;
}

namespace MAXIV::GunTestFacility
{

  /*!
    \class BuildingB
    \version 1.0
    \author Konstantin Batkov
    \date August 2023
    \brief MAX IV building B (housing the gun test facility)
  */

  class BuildingB : public attachSystem::ContainedComp,
	       public attachSystem::FixedRotate,
	       public attachSystem::CellMap,
	       public attachSystem::SurfMap
  {
  private:

    double gunRoomLength;         ///< gun test room inner length
    double gunRoomWidth;          ///< gun test room inner width
    double klystronRoomWidth;     ///< klystron test room room width
    double height;                ///< distance from beam line to inner roof plane of the gun test room
    double hallHeight;            ///< hall height (except gun test room)
    double depth;                 ///< beam line height
    double eastClearance;         ///< gap between building B and B1
    double backWallThick;         ///< thickness of the back wall
    double backWallCornerCut;     ///< back wall corner cut length
    double gunRoomEntranceWidth;  ///< gun test room entrance width
    double midWallThick;          ///< thickness of the middle wall
    double outerWallThick;        ///< outer wall thickness
    double internalWallThick;     ///< internal wall thickness
    double mazeWidth;             ///< maze (sluss) width
    double mazeEntranceOffset;    ///< offset of the maze entrance door
    double mazeEntranceWidth;     ///< width of the maze entrance door
    double mazeEntranceHeight;    ///< height of the maze entrance door
    double hallLength;            ///< max Förrum length
    double floorThick;            ///< floor thickness
    double roofGunTestThick;      ///< roof thickness above the gun test room

    double trspRoomWidth;         ///< TRSP room width
    double stairRoomWidth;        ///< stairway room width
    double stairRoomLength;       ///< stairway room length
    double elevatorWidth;        ///< elevator room width
    double elevatorLength;       ///< elevator room length
    double oilRoomEntranceWidth;  ///< oil room entrance width
    double oilRoomWallThick;      ///< thickness of the oil room walls
    double controlRoomWidth;      ///< control room width
    double controlRoomLength;     ///< control room length
    double controlRoomWallThick;  ///< control room wall thickness
    double controlRoomEntranceWidth; ///< control room entrance width
    double controlRoomEntranceOffset; ///< control room entrance offset along the wall

    double level9Height;          ///< second floor full height (level 9)
    double level9RoofThick;       ///< second floor (level 9) roof thickness
    double level9VentDuctShieldLength; ///< ventillation ducts shield length (above gun test room)
    double level9VentDuctShieldWidth; ///< ventillation ducts shield width
    double level9VentDuctShieldHeight; ///< ventillation ducts shield height
    double level9VentDuctShieldThick; ///< ventillation ducts shield thickness
    double level9VentDuctShieldOffset; ///< ventillation ducts shield offset from the east wall

    int wallMat;                  ///< Wall material
    int voidMat;                  ///< void material
    int oilRoomWallMat;           ///< material of the oil room walls
    int level9VentDuctShieldMat;  ///< ventillation ducts shield material
  int doorBricksMat;            ///< Material of the lead bricks

    std::shared_ptr<xraySystem::Duct> ductWave; // wave guide
    std::shared_ptr<xraySystem::Duct> ductSignal1; // electrical duct
    std::shared_ptr<xraySystem::Duct> ductSignal2; // electrical duct
    std::shared_ptr<xraySystem::Duct> ductSignal3;
    std::shared_ptr<xraySystem::Duct> ductWater1; // small copper non-insulated water pipes
    std::shared_ptr<xraySystem::Duct> ductVent; // just a ventillation penetration
    std::shared_ptr<xraySystem::Duct> ductLaser; // laser penetration
    std::shared_ptr<xraySystem::Duct> ductSignal4; // electrical duct
    std::shared_ptr<xraySystem::Duct> ductWater2; // water inlet and outlet, heat insulated
    std::shared_ptr<xraySystem::Duct> ductSuction; // duct for the soldering iron suction fan
    std::shared_ptr<xraySystem::Duct> ductVentRoof1; // ventillation @ roof
    std::shared_ptr<xraySystem::Duct> ductVentRoof2; // ventillation @ roof

    std::shared_ptr<ConcreteDoor> door; // back wall concrete door
    double doorBricksThick;       ///< Thickness of lead bricks at the concrete door base
  double doorBricksHeight;      ///< Height of lead bricks at the concrete door base
  double doorBricksLength;      ///< Length of lead bricks at the concrete door base
  double doorBricksOffset;      ///< Offset of the lead bricks from the north wall

    void populate(const FuncDataBase&);
    void createSurfaces();
    void createObjects(Simulation&);
    void createLinks();
    void createDucts(Simulation&);

  public:

    BuildingB(const std::string&);
    BuildingB(const BuildingB&);
    BuildingB& operator=(const BuildingB&);
    virtual BuildingB* clone() const;
    virtual ~BuildingB();

    using attachSystem::FixedComp::createAll;
    void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

  };

}

#endif
