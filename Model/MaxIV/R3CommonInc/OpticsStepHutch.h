/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3CommonInc/OpticsStepHutch.h
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
#ifndef xraySystem_OpticsStepHutch_h
#define xraySystem_OpticsStepHutch_h

class Simulation;

namespace xraySystem
{
  class PortChicane;
/*!
  \class OpticsStepHutch
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief OpticsStepHutch unit  

  Built around the central beam axis
*/

class OpticsStepHutch :
    public OpticsHutch
{
 private:
  
  double ringStepLength;          ///< Distance down hutch to step
  double ringStepWidth;           ///< Width of step from centre line
  
  void populate(const FuncDataBase&) override;
  void createSurfaces() override;
  void createObjects(Simulation&) override;
  virtual void createLinks();

 public:

  OpticsStepHutch(const std::string&);
  OpticsStepHutch(const OpticsStepHutch&);
  OpticsStepHutch& operator=(const OpticsStepHutch&);
  ~OpticsStepHutch() override;

};

}

#endif
 
