/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/danmaxOpticsHutch.h
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
#ifndef xraySystem_danmaxOpticsHutch_h
#define xraySystem_danmaxOpticsHutch_h

class Simulation;

namespace xraySystem
{
  class PortChicane;
/*!
  \class danmaxOpticsHutch
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief danmaxOpticsHutch unit  

  Built around the central beam axis
*/

class danmaxOpticsHutch :
    public OpticsHutch
{
 private:
  
  double ringStepLength;          ///< Distance down hutch to step
  double ringStepWidth;           ///< Width of step from centre line
  
  virtual void populate(const FuncDataBase&);
  virtual void createSurfaces();
  virtual void createObjects(Simulation&);
  virtual void createLinks();

 public:

  danmaxOpticsHutch(const std::string&);
  danmaxOpticsHutch(const danmaxOpticsHutch&);
  danmaxOpticsHutch& operator=(const danmaxOpticsHutch&);
  virtual ~danmaxOpticsHutch();

};

}

#endif
 
