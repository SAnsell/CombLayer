/********************************************************************* 
  Comb-Layer : MCNP(X) Input builder
 
 * File:   physicsInc/PWTConstructor.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef physicsSystem_PWTConstructor_h
#define physicsSystem_PWTConstructor_h

namespace attachSystem
{
  class FixedComp;
}




namespace physicsSystem
{
  class PhysicsCards;
  class PWTControl;
  
/*!
  \class PWTConstructor
  \version 1.0
  \author S. Ansell
  \date May 2015
  \brief Processes and method of producing Ext card input

*/

class PWTConstructor 
{
 private:

  /// Ranges to build
  groupRange Zones;
  //  std::vector<MapSupport::Range<int>> Zones;

  static bool getVector(const objectGroups&,
			const std::vector<std::string>&,
			const size_t,Geometry::Vec3D&);

  bool procZone(const objectGroups&,std::vector<std::string>&);
  bool procType(std::vector<std::string>&,PWTControl&);
  
  void writeHelp(std::ostream&) const;
    
 public:

  PWTConstructor();
  PWTConstructor(const PWTConstructor&);
  PWTConstructor& operator=(const PWTConstructor&);
  ~PWTConstructor() {}  ///< Destructor

  void processUnit(SimMCNP&,
		   const mainSystem::inputParam&,
		   const size_t);
};

}

#endif
 
