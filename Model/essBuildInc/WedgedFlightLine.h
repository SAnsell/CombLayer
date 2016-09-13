/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/WedgedFlightLine.h
 *
 * Copyright (c) 2016 by Stuart Ansell / Konstantin Batkov
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
#ifndef essSystem_WedgedFlightLine_h
#define essSystem_WedgedFlightLine_h

class Simulation;

namespace moderatorSystem
{
  class TaperedFlightLine;
}

namespace essSystem
{

/*!
  \class WedgedFlightLine
  \version 1.0
  \author Konstantin Batkov
  \date Sep 2016
  \brief WedgedFlightLine: Tapered flight line with wedges
*/

class WedgedFlightLine : public moderatorSystem::TaperedFlightLine
{
 private:
  
  const int flightIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index
    

 public:

  WedgedFlightLine(const std::string&);
  //  WedgedFlightLine(const WedgedFlightLine&);
  //  WedgedFlightLine& operator=(const WedgedFlightLine&);
  ~WedgedFlightLine();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int);

};

}

#endif
 
