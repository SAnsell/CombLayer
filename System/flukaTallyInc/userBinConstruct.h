/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/userBinConstruct.h
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
#ifndef tallySystem_userBinConstruct_h
#define tallySystem_userBinConstruct_h

namespace attachSystem
{
  class FixedComp;
}

namespace mainSystem
{
  class inputParam;
}

class Simulation;

namespace tallySystem
{

/*!
  \class userBinConstruct
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief Constructs a mesh tally from inputParam
*/

class userBinConstruct 
{
  private:
  
  /// Private constructor
  userBinConstruct() {}

  createTally(SimFLUKA&,
	      const std::string&,
	      const int,
	      const Geometry::Vec3D&,const Geometry::Vec3D&
	      const std::array<size_t,3>&);
  

  static int convertTallyType(const std::string&);
  
 public:

  static void processMesh(SimFLUKA&,const mainSystem::inputParam&
			  const size_t);
  
  static void writeHelp(std::ostream&);
};

}

#endif
 
