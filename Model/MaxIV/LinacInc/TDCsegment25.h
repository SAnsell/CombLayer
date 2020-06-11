/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/TDCsegment25.h
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
#ifndef tdcSystem_TDCsegment25_h
#define tdcSystem_TDCsegment25_h

/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace tdcSystem
{
  class FlatPipe;
  class TriPipe;

  /*!
    \class TDCsegment25
    \version 1.0
    \author S. Ansell
    \date June 2020
    \brief Dividing segment in the TDC from the linac
  */

class TDCsegment25 :
  public TDCsegment
{
 private:

  /// first pipe
  std::shared_ptr<tdcSystem::TriPipe> triPipeA;

  /// first  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleA;


  void buildObjects(Simulation&);
  void createLinks();

 public:

  TDCsegment25(const std::string&);
  TDCsegment25(const TDCsegment25&);
  TDCsegment25& operator=(const TDCsegment25&);
  ~TDCsegment25();


  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
