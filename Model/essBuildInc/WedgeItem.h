/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/WedgeItem.h
*
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef essSystem_WedgeItem_h
#define essSystem_WedgeItem_h

class Simulation;

namespace essSystem
{

/*!
  \class WedgeItem
  \author S. Ansell / K. Batkov
  \version 1.0
  \date September 2016
  \brief Flight line Wedge Item
*/

class WedgeItem : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:

  const int wedgeIndex;          ///< Index of surface offset
  int cellIndex;                 ///< Cell index

  double innerR;                 ///< Radius of inner cylinder (calculated in createSurfaces)
  double length;                 ///< Length
  double baseWidth;              ///< Base width

  int mat;                       ///< material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces(const attachSystem::FixedComp&, const int);
  void createObjects(Simulation&,
		     const attachSystem::FixedComp&,const int,
		     const attachSystem::FixedComp&, const int, const int);
  void createLinks(const attachSystem::FixedComp&);

 public:

  WedgeItem(const std::string&);
  WedgeItem(const WedgeItem&);
  WedgeItem& operator=(const WedgeItem&);
  virtual ~WedgeItem();

  void createAll(Simulation&,const attachSystem::FixedComp&, const int,
		 const attachSystem::FixedComp&, const int, const int);
  
};

}

#endif
 
