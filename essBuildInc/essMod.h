/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/essMod.h
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
#ifndef essSystem_essMod_h
#define essSystem_essMod_h

class Simulation;

namespace essSystem
{

/*!
  \class essMod
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief essMod [insert object]
*/

class essMod : public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp
{
 public:

  essMod(const std::string&);
  essMod(const essMod&);
  essMod& operator=(const essMod&);
  virtual essMod* clone() const =0;
  virtual ~essMod();

  virtual int getMainBody() const =0;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&) =0;


};

}

#endif
 
