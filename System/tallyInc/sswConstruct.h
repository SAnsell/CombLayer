/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/sswConstruct.h
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
#ifndef tallySystem_sswConstruct_h
#define tallySystem_sswConstruct_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;
class SimMNCP;

namespace tallySystem
{

/*!
  \class sswConstruct
  \version 1.0
  \author S. Ansell
  \date October 2016
  \brief Constructs an SSW tally 

*/

class sswConstruct 
{
 private:

  /// private constructor
  sswConstruct() {}

 public:

  static void processSSW(SimMCNP&,const mainSystem::inputParam&,
			 const size_t);

  static void writeHelp(std::ostream&);
};

}

#endif
 
