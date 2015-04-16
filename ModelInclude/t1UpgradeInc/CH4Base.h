/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/CH4Base.h
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
#ifndef ts1System_CH4Base_h
#define ts1System_CH4Base_h

class Simulation;

namespace ts1System
{

/*!
  \class CH4Base
  \version 1.0
  \author S. Ansell
  \date July 2013
  \brief TS1 CH4Base upgrade
*/

class CH4Base : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 protected:
  
  const int ch4Index;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
 public:

  CH4Base(const std::string&,const size_t);
  CH4Base(const CH4Base&);
  CH4Base& operator=(const CH4Base&);
  virtual CH4Base* clone() const =0;
  virtual ~CH4Base();

  virtual std::string getComposite(const std::string&) const =0;
  //  virtual void addToInsertChain(attachSystem::ContainedComp&) const =0;

  virtual void createAll(Simulation&,const attachSystem::FixedComp&) =0;

};

}

#endif
 
