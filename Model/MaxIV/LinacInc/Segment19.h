/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment19.h
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
#ifndef tdcSystem_Segment19_h
#define tdcSystem_Segment19_h

namespace tdcSystem
{
  /*!
    \class Segment19
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 15
  */

class Segment19 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;       ///< #1 Bellow
  std::shared_ptr<constructSystem::PipeTube> gauge;        ///< #3 Vacuum gauge PTR18751
  std::shared_ptr<constructSystem::GateValveCube> gateA;   ///< #4 Fast closing valve
  std::shared_ptr<constructSystem::PipeTube> ionPump;     ///< #4 Ion pump 45l cf40
  std::shared_ptr<xraySystem::CylGateValve> gateB;   ///< #6 Gate valve 48132 CE44
  std::shared_ptr<constructSystem::Bellows> bellowB;       ///< #1 Bellow

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment19(const std::string&);
  Segment19(const Segment19&);
  Segment19& operator=(const Segment19&);
  ~Segment19();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
