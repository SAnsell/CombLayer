/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/EssModBase.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell/K. Batkov
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
#ifndef essSystem_EssModBase_h
#define essSystem_EssModBase_h

class Simulation;

namespace essSystem
{

/*!
  \class EssModBase
  \version 1.0
  \author S. Ansell
  \date November 2017
  \brief General EssModBase unit
*/

class EssModBase :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap
{
 protected:
  
  void createUnitVector(const attachSystem::FixedComp&,
			const long int,
			const attachSystem::FixedComp&,
			const long int) override;
  
 public:

  EssModBase(const std::string&,const size_t);
  EssModBase(const EssModBase&);
  EssModBase& operator=(const EssModBase&);
  virtual EssModBase* clone() const =0;
  ~EssModBase() override;

  virtual const attachSystem::FixedComp&
    getComponent(const std::string&) const;
  
};

}

#endif
 
