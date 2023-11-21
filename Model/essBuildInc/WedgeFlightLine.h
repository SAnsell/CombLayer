/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/WedgeFlightLine.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell / Konstantin Batkov
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
#ifndef essSystem_WedgeFlightLine_h
#define essSystem_WedgeFlightLine_h

class Simulation;

namespace moderatorSystem
{
  class BasicFlightLine;
}

namespace essSystem
{

class WedgeItem;
 
/*!
  \class WedgeFlightLine
  \version 1.0
  \author Konstantin Batkov
  \date September 2016
  \brief Tapered flight line with wedges
*/


  
class WedgeFlightLine :
    public constructSystem::BasicFlightLine
{
 private:
  
  size_t nWedges;                  ///< number of wedges to build
  std::vector<std::shared_ptr<WedgeItem> > wedges; ///< array of wedges

  void populate(const FuncDataBase&) override;
  void buildWedges(Simulation&);
  
 public:

  WedgeFlightLine(const std::string&);
  WedgeFlightLine(const WedgeFlightLine&);
  WedgeFlightLine& operator=(const WedgeFlightLine&);
  ~WedgeFlightLine() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;


};

}

#endif
 
