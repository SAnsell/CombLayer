/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/WheelBase.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef essSystem_WheelBase_h
#define essSystem_WheelBase_h

class Simulation;

namespace essSystem
{

/*!
  \class WheelBase
  \author S. Ansell
  \version 1.0
  \date Septebmer 2013
  \brief Specialized for a Wheel system
*/

class WheelBase :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedGroup,
    public attachSystem::CellMap
{
 protected:

  int engActive;                 ///< Engineering active flag

  public:

  WheelBase(const std::string&);
  WheelBase(const WheelBase&);
  WheelBase& operator=(const WheelBase&);
  ~WheelBase() override;

  void setEngActive() { engActive=1; }
  ///\cond ABSTRACT

  virtual WheelBase* clone() const =0;
  virtual double wheelHeight() const =0;

  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override =0;
  ///\endcond ABSTRACT
};

}

#endif
 

