/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/virtualMod.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef delftSystem_virtualMod_h
#define delftSystem_virtualMod_h

class Simulation;

namespace attachSystem
{
  class TwinComp;
}

namespace delftSystem
{

/*!
  \class virtualMod
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief virtualMod [insert object]
*/

class virtualMod : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 public:

  virtualMod(const std::string&);
  virtualMod(const virtualMod&);
  virtualMod& operator=(const virtualMod&);
  virtual ~virtualMod();

  ///\cond ABSTRACT 
  virtual virtualMod* clone() const =0;
  virtual int getMainBody() const =0;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) =0;
  virtual void postCreateWork(Simulation&) =0;
  ///\endcond ABSTRACT 
};

}

#endif
 
