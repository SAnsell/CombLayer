/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimaxInc/softimaxFrontEnd.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_softimaxFrontEnd_h
#define xraySystem_softimaxFrontEnd_h

/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace xraySystem
{

  class UTubePipe;
  class Undulator;


  /*!
    \class softimaxFrontEnd
    \version 1.0
    \author S. Ansell
    \date March 2018
    \brief General constructor front end optics
  */

class softimaxFrontEnd :
  public R3FrontEnd
{
 private:

  /// Pipe in undulator
  std::shared_ptr<xraySystem::UTubePipe> undulatorPipe;
  /// Undulator in vacuum box
  std::shared_ptr<xraySystem::Undulator> undulator;

  virtual const attachSystem::FixedComp&
    buildUndulator(Simulation&,MonteCarlo::Object*,
		   const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void buildObjects(Simulation&);
  virtual void createLinks();

 public:

  softimaxFrontEnd(const std::string&);
  softimaxFrontEnd(const softimaxFrontEnd&);
  softimaxFrontEnd& operator=(const softimaxFrontEnd&);
  virtual ~softimaxFrontEnd();

};

}

#endif
