/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   epbBuildInc/Building.h
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
#ifndef epbSystem_Building_h
#define epbSystem_Building_h

class Simulation;

namespace epbSystem
{
/*!
  \class Building
  \version 1.0
  \author S. Ansell
  \date September 2013

  \brief Building for EPB Hall
*/

class Building : public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:

  double height;               ///< Base of roof
  double depth;                ///< Top of floor level
  double width;                ///< Full width
  double length;               ///< Full length

  double floorThick;             ///< Index of sector
  double wallThick;               ///< Full length
  double roofThick;               ///< Full length

  int concMat;                    ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Building(const std::string&);
  Building(const Building&);
  Building& operator=(const Building&);
  virtual ~Building();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
