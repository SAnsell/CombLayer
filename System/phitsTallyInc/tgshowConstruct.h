/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/tgshowConstruct.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
#ifndef phitsSystem_tgshowConstruct_h
#define phitsSystem_tgshowConstruct_h

namespace attachSystem
{
  class FixedComp;
}

namespace mainSystem
{
  class inputParam;
}

class Simulation;

namespace phitsSystem
{

/*!
  \class tgshowConstruct
  \version 1.0
  \author S. Ansell
  \date September 2018
  \brief Constructs a mesh tally from inputParam
*/

class tgshowConstruct 
{
  private:
  
  /// Private constructor
  tgshowConstruct() {}

  static void createTally(SimPHITS&,
			  const std::string&,
			  const int,
			  const Geometry::Vec3D&,const Geometry::Vec3D&,
			  const std::array<size_t,3>&);
  

  static std::string convertTallyType(const std::string&);
  
 public:

  static void processMesh(SimPHITS&,const mainSystem::inputParam&,
			  const size_t);
  
  static void writeHelp(std::ostream&);
};

}

#endif
 
