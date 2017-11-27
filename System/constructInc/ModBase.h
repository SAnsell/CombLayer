/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/ModBase.h
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
#ifndef constructSystem_ModBase_h
#define constructSystem_ModBase_h

class Simulation;

namespace constructSystem
{

/*!
  \class ModBase
  \version 1.0
  \author S. Ansell
  \date July 2013
  \brief General ModBase unit
*/

class ModBase : public attachSystem::ContainedComp,
  public attachSystem::LayerComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 protected:
  
  const int modIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  std::vector<long int> flightSides;  ///< Sides for flight inde

  virtual void populate(const FuncDataBase&);
  virtual void createUnitVector(const attachSystem::FixedComp&,
				const attachSystem::FixedComp*,
				const long int);
  
 public:

  ModBase(const std::string&,const size_t);
  ModBase(const ModBase&);
  ModBase& operator=(const ModBase&);
  virtual ModBase* clone() const =0;
  virtual ~ModBase();

  std::string getComposite(const std::string&) const;

  virtual const attachSystem::FixedComp&
    getComponent(const std::string&) const;
  long int getSideIndex(const size_t) const;

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const attachSystem::FixedComp* =0,
			 const long int =0) =0;

};

}

#endif
 
