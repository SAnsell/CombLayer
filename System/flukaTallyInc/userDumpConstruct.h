/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/userDumpConstruct.h
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
#ifndef tallySystem_userDumpConstruct_h
#define tallySystem_userDumpConstruct_h

namespace attachSystem
{
  class FixedComp;
}

namespace mainSystem
{
  class inputParam;
}

class Simulation;

namespace flukaSystem
{

/*!
  \class userDumpConstruct
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief Constructs a mesh tally from inputParam
*/

class userDumpConstruct 
{
  private:
  
  /// Private constructor
  userDumpConstruct() {}

  static void createTally(SimFLUKA&,const int,
			  const int,const std::string&);
  

  static int convertTallyType(const std::string&);
  
 public:

  static void processDump(SimFLUKA&,const mainSystem::inputParam&,
			  const size_t);
  
  static void writeHelp(std::ostream&);
};

}

#endif
 
