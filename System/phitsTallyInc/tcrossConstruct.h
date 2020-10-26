/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/tcrossConstruct.h
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
#ifndef phitsSystem_tcrossConstruct_h
#define phitsSystem_tcrossConstruct_h

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
  \class tcrossConstruct
  \version 1.0
  \author S. Ansell
  \date October 2020
  \brief Constructs a t-cross tally from inputParam
*/

class tcrossConstruct 
{
  private:
  
  /// Private constructor
  tcrossConstruct() {}

  static void createTally(SimPHITS&,const std::string&,
			  const int,const int,const int,
			  const size_t,const double,const double,
			  const size_t,const double,const double);

  static std::string convertTallyType(const std::string&);
  
 public:

  static void processSurface(SimPHITS&,const mainSystem::inputParam&,
			     const size_t);
  
  static void writeHelp(std::ostream&);
};

}

#endif
 
