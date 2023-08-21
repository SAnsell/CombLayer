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
#ifndef xraySystem_GunTestFacilityHall_h
#define xraySystem_GunTestFacilityHall_h

class Simulation;

namespace MAXIV::GunTestFacility
{

  /*!
    \class Hall
    \version 1.0
    \author Konstantin Batkov
    \date August 2023
    \brief Gun Test Facility building
  */

  class Hall : public attachSystem::ContainedComp,
	       public attachSystem::FixedRotate,
	       public attachSystem::CellMap,
	       public attachSystem::SurfMap
  {
  private:

    double mainRoomLength;        ///< inner main room length
    double mainRoomWidth;         ///< inner main room width
    double klystronRoomWidth;          ///< lab room width
    double height;                ///< distance from beam line to inner roof plane
    double depth;                 ///< beam line height
    double backWallThick;         ///< thickness of the back wall
    double backWallLength;        ///< back wall length
    double midWallThick;          ///< thickness of the middle wall
    double outerWallThick;        ///< outer wall thickness
    double klystronRoomWallThick;      ///< lab room wall thickness
    double mazeWidth;             ///< maze width

    int wallMat;                  ///< Wall material
    int voidMat;                  ///< void material

    void populate(const FuncDataBase&);
    void createSurfaces();
    void createObjects(Simulation&);
    void createLinks();

  public:

    Hall(const std::string&);
    Hall(const Hall&);
    Hall& operator=(const Hall&);
    virtual Hall* clone() const;
    virtual ~Hall();

    using attachSystem::FixedComp::createAll;
    void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

  };

}

#endif
