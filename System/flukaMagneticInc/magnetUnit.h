/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcessInc/magnetUnit.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef flukaSystem_magnetUnit_h
#define flukaSystem_magnetUnit_h

namespace flukaSystem
{
  
/*!
  \class magnetUnit
  \version 1.0
  \date February 2019
  \author S.Ansell
  \brief Holds an external magnetic system

  FixedOffset holds the basis set for the axial components
*/

class magnetUnit :
  public attachSystem::FixedOffset
{
 protected:

  size_t index;           ///< Order index for magnetic field

  double length;          ///< length of magnetic unit 
  double width;           ///< width of magnetic unit
  double height;          ///< height of magnetic unit

  /// active cells
  std::set<int> activeCells; 

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
 public:

  magnetUnit(const std::string&,const size_t);
  magnetUnit(const magnetUnit&);
  magnetUnit& operator=(const magnetUnit&);
  virtual ~magnetUnit();

  void writeFLUKA(std::ostream&) const;
};

}

#endif
