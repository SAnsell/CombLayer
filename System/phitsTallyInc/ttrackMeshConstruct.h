/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/ttrackMeshConstruct.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef phitsSystem_ttrackMeshConstruct_h
#define phitsSystem_ttrackMeshConstruct_h

namespace attachSystem
{
  class FixedComp;
}

namespace mainSystem
{
  class inputParam;
}

class Simulation;
class SimPHITS;

namespace phitsSystem
{

/*!
  \class ttrackMeshConstruct
  \version 1.0
  \author S. Ansell
  \date October 2020
  \brief Constructs a t-cross tally from inputParam
*/

class ttrackMeshConstruct 
{
  private:
  
  /// Private constructor
  ttrackMeshConstruct() {}

  static void createTally(SimPHITS&,const std::string&,
			  const int,
			  const Geometry::Vec3D&,
			  const Geometry::Vec3D&,
			  const std::array<size_t,3>&);

			  
  static std::string convertParticleType(const std::string&);
  
 public:

  static void processMesh(SimPHITS&,const mainSystem::inputParam&,
			     const size_t);
  
  static std::string writeHelp();
};

}

#endif
 
