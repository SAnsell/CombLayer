/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/GunTestFacility.h
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
#ifndef GunTestFacility_h
#define GunTestFacility_h

namespace MAXIV
{
  class GTFLine;

  namespace GunTestFacility
  {
    /*!
      \class GunTestFacility
      \version 1.0
      \author K. Batkov
      \date August 2023
      \brief Manager of the Gun Test Facility build
    */

    class BuildingB;

    class GunTestFacility :
      public attachSystem::FixedOffset,
      public attachSystem::CellMap
    {
    private:

      std::shared_ptr<BuildingB> buildingB;
      std::shared_ptr<MAXIV::GTFLine>    beamLine;

    public:

      GunTestFacility(const std::string&);
      GunTestFacility(const GunTestFacility&);
      GunTestFacility& operator=(const GunTestFacility&);
      virtual ~GunTestFacility();

      void createAll(Simulation&,const attachSystem::FixedComp&,const long int);
    };

  }
}

#endif
