/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/RElement.h
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
#ifndef delftSystem_RElement_h
#define delftSystem_RElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class RElement
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief Element object of a reactors

  Holds a general element object of a reactor
*/

class RElement  :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 protected:

  const std::string baseName;  ///< Base block name
  const size_t XIndex;         ///< Index of block [X]
  const size_t YIndex;         ///< Index of block [Z]

  const FuelLoad* FuelPtr;     ///< Fuel load (or null)
  int insertCell;               ///< Cell to insert into

  void populate(const FuncDataBase&) override;
  
 public:

  RElement(const size_t,const size_t,const std::string&);
  RElement(const RElement&);
  RElement& operator=(const RElement&);
  ~RElement() override {}   

  /// set fuel load
  void setFuelLoad(const FuelLoad& FL) { FuelPtr=&FL; }
    
  virtual std::string getItemKeyName() const;  

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override =0;


};

}  

#endif
