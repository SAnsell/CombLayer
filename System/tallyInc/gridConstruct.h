/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/gridConstruct.h
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
#ifndef tallySystem_gridConstruct_h
#define tallySystem_gridConstruct_h

namespace attachSystem
{
  class FixedComp;
}

class objectGroups;
class Simulation;
class SimMCNP;

namespace tallySystem
{

/*!
  \class gridConstruct
  \version 1.0
  \author S. Ansell
  \date Januray 2015
  \brief Construct grid tallies

*/

class gridConstruct 
{
 private:

  static void applyMultiGrid(SimMCNP&,const size_t,const size_t,
		      const Geometry::Vec3D&,
		      const Geometry::Vec3D&,
		      const Geometry::Vec3D&);

  static int calcGlobalCXY(const objectGroups&,
			   const std::string&,const std::string&,
			   Geometry::Vec3D&,Geometry::Vec3D&,
			   Geometry::Vec3D&);

  /// private constructor 
  gridConstruct() {};
 
 public:

  
  // Point Stuff
  static void processGrid(SimMCNP&,const mainSystem::inputParam&,
			  const size_t);


  static void writeHelp(std::ostream&);

};

}

#endif
 
