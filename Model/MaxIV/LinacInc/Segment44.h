/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment44.h
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
#ifndef tdcSystem_Segment44_h
#define tdcSystem_Segment44_h

namespace tdcSystem
{
  class TriGroup;
  class CurveMagnet;
  /*!
    \class Segment44
    \version 1.0
    \author Stuart Ansell
    \date Auguse 2020
    \brief SPF segment 44
  */

class Segment44 :
  public TDCsegment
{
 private:

  std::shared_ptr<tdcSystem::TriGroup> triBend;  ///< vacuum part of bending mag
  std::shared_ptr<tdcSystem::CurveMagnet> cMag;  ///< Main beam dump magnet (DIP BD2)

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment44(const std::string&);
  Segment44(const Segment44&);
  Segment44& operator=(const Segment44&);
  ~Segment44();

  void writePoints() const override;
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
