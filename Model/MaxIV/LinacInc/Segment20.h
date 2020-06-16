/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment20.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef tdcSystem_Segment20_h
#define tdcSystem_Segment20_h

namespace tdcSystem
{
  class TWCavity;
  /*!
    \class Segment20
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 15
  */

class Segment20 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;  ///< front flange
  std::shared_ptr<tdcSystem::TWCavity> cavity;         ///< TDC cavity section
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;  ///< back flange

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment20(const std::string&);
  Segment20(const Segment20&);
  Segment20& operator=(const Segment20&);
  ~Segment20();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
