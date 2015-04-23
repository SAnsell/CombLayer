/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   constructInc/Target.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef constructSystem_Target_h
#define constructSystem_Target_h

class Simulation;

namespace ts1System
{
  class BeamWindow;
}

namespace constructSystem
{
/*!
  \class Target
  \version 1.0
  \author S. Ansell
  \date January 2010
  \brief Abstract top-leve target

  Provides linkage to its outside on FixedComp[0]
*/

class Target : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
 public:

  TS2target();
  TS2target(const TS2target&);
  TS2target& operator=(const TS2target&);
  virtual TS2target* clone() const =0; 
  virtual ~TS2target();


  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&) =0;
  virtual void addProtonLine(Simulation&,
			     const attachSystem::FixedComp&,
			     const long int) =0;

};

}

#endif
 
