/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/CH4PreModBase.h
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
#ifndef ts1System_CH4PreModBase_h
#define ts1System_CH4PreModBase_h

class Simulation;

namespace ts1System
{

/*!
  \class CH4PreModBase
  \version 1.0
  \author S. Ansell
  \date July 2013
  \brief Pre-mod genertic for all CH4 premods
*/

class CH4PreModBase : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 protected:
  
  const int preIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  int voidCell;                 ///< void cell [outer]
  
 public:

  CH4PreModBase(const std::string&,const size_t);
  CH4PreModBase(const CH4PreModBase&);
  CH4PreModBase& operator=(const CH4PreModBase&);
  virtual ~CH4PreModBase();
  virtual CH4PreModBase* clone() const =0; 

  int getVoidCell() const { return voidCell; }

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const size_t,const size_t) =0;

};

}

#endif
 
