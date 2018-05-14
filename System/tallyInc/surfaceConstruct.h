/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/surfaceConstruct.h
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
#ifndef tallySystem_surfaceConstruct_h
#define tallySystem_surfaceConstruct_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;
class SimMCNP;

namespace tallySystem
{

/*!
  \class surfaceConstruct 
  \version 1.0
  \author S. Ansell
  \date April 2012
  \brief Holds everthing for tallies

  Provides linkage to its outside on FixedComp[0]
*/

class surfaceConstruct 
{
 private:

  static int processSurfObject(SimMCNP&,const int,
			       const std::string&,
			       const long int,
			       const std::vector<std::string>&);
  
  static int processSurfMap(SimMCNP&,const int,
			    const std::string&,
			    const std::string&,
			    const long int);

  static void processSurface(SimMCNP&,const int,
			    const mainSystem::inputParam&,
			    const size_t);

  /// private constructor 
  surfaceConstruct();
  
 public:




  static void processSurfaceCurrent(SimMCNP&,const mainSystem::inputParam&,
			       const size_t);
  static void processSurfaceFlux(SimMCNP&,const mainSystem::inputParam&,
			    const size_t);

  static void writeHelp(std::ostream&);
};

}

#endif
 
