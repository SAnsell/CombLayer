/********************************************************************* 
  Comb-Layer : MCNP(X) Input builder
 
 * File:   physicsInc/ZoneUnit.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef physicsSystem_ZoneUnit_h
#define physicsSystem_ZoneUnit_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;

namespace physicsSystem
{

  
/*!
  \class ZoneUnit
  \version 1.0
  \author S. Ansell
  \date February 2016
  \brief Adds zone (ranged int) support 
  \tparam 
*/
template<typename T>
class ZoneUnit 
{
 public:

  /// Ranges to build
  std::vector<MapSupport::Range<int>> Zones;
  /// Data associated with the zone
  std::vector<T> ZoneData;

  static MapSupport::Range<int>
    createMapRange(std::vector<int>&);

  void sortZone();
  bool procZone(std::vector<std::string>&);
  void addData(const T&);
    
  ZoneUnit();
  ZoneUnit(const ZoneUnit&);
  ZoneUnit& operator=(const ZoneUnit&);
  ~ZoneUnit() {}  ///< Destructor

  size_t findItem(const int) const;
  bool inRange(const int,T&) const;
  
};

}

#endif
 
