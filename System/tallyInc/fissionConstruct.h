/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/fissionConstruct.h
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
#ifndef tallySystem_fissionConstruct_h
#define tallySystem_fissionConstruct_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;
class SimMCNP;

namespace tallySystem
{

/*!
  \class fissionConstruct
  \version 1.0
  \author S. Ansell
  \date April 2014
  \brief Fission cell tally 

  Similar to flux tally
*/

class fissionConstruct 
{
 private:

  static int convertRange(const std::string&,int&,int&);

  /// private constructor
  fissionConstruct() {}
  
 public:



  static int processPower(SimMCNP&,const mainSystem::inputParam&,
			   const size_t);

  static void writeHelp(std::ostream&);
};

}

#endif
 
