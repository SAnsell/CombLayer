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
    double height;                ///< distance from beam line to inner roof plane
    double depth;                 ///< beam line height
    double backWallThick;         ///< thickness of the back wall
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
    double roof1Thick;            ///< foor 1 roof thickness

    double trspRoomWidth;         ///< TRSP room width
    double stairRoomWidth;        ///< stairway room width
    double stairRoomLength;       ///< stairway room length
    double elevatorWidth;        ///< elevator room width
    double elevatorLength;       ///< elevator room length

    int wallMat;                  ///< Wall material
    int voidMat;                  ///< void material

    std::shared_ptr<xraySystem::Duct> duct;

    void populate(const FuncDataBase&);
    void createSurfaces();
    void createObjects(Simulation&);
    void createLinks();

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
