/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/TDCsegment20.h
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
#ifndef tdcSystem_TDCsegment20_h
#define tdcSystem_TDCsegment20_h

namespace tdcSystem
{
  class TWCavity;
  /*!
    \class TDCsegment20
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 15
  */

class TDCsegment20 :
  public TDCsegment
{
 private:

  std::shared_ptr<tdcSystem::TWCavity> cavity;      ///< TDC cavity section

  void buildObjects(Simulation&);
  void createLinks();

 public:

  TDCsegment20(const std::string&);
  TDCsegment20(const TDCsegment20&);
  TDCsegment20& operator=(const TDCsegment20&);
  ~TDCsegment20();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
