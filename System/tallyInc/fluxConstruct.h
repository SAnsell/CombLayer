/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/fluxConstruct.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef tallySystem_fluxConstruct_h
#define tallySystem_fluxConstruct_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;

namespace tallySystem
{

/*!
  \class fluxConstruct
  \version 1.0
  \author S. Ansell
  \date April 2012
  \brief Holds everthing for tallies

  Provides linkage to its outside on FixedComp[0]
*/

class fluxConstruct 
{
 private:
  
  std::vector<int> getCell(const Simulation&,
			   const mainSystem::inputParam&,
			   const size_t,
			   const int);
 public:

  fluxConstruct();
  fluxConstruct(const fluxConstruct&);
  fluxConstruct& operator=(const fluxConstruct&);
  virtual ~fluxConstruct() {}  ///< Destructor

  int processFlux(Simulation&,const mainSystem::inputParam&,
		  const size_t) const;

  virtual void writeHelp(std::ostream&) const;
};

}

#endif
 
