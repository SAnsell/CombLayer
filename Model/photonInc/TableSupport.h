/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/TableSupport.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef photonSystem_TableSupport_h
#define photonSystem_TableSupport_h

class Simulation;

namespace constructSystem
{
  class RingSeal;
  class RingFlange;
}

namespace photonSystem
{
  
/*!
  \class TableSupport
  \author S. Ansell
  \version 1.0
  \date October 2016
  \brief Specialized for a vacuum vessel

  Cylindrical with semi-spherical front/back Doors
*/

class TableSupport : 
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:

  double length;              ///< Outer thickenss
  double width;               ///< width of back volume
  double height;              ///< height of back volume
 
  int mat;                     ///< Outer material
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  TableSupport(const std::string&);
  TableSupport(const TableSupport&);
  TableSupport& operator=(const TableSupport&);
  ~TableSupport() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
};

}

#endif
 
