/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/WedgeItem.h
 *
 * Copyright (c) 2004-2019 by Konstantin Batkov
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
    public attachSystem::FixedOffsetUnit
{
 private:

  double length;                 ///< Length
  double baseWidth;              ///< Base width
  double tipWidth;               ///< Tip width
  ///  Engineering angle with respect to (0,0) in TSC.
  ///  Calculated counterclockwise from OY. Used to set XYAngle
  double theta;                  

  int mat;                       ///< material

  // aux variables, non-populated
  Geometry::Cylinder *outerCyl;  ///< outer cylinder (base surface)

  // Functions:

  double getFixedXYAngle(const double) const;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces(const attachSystem::FixedComp&,const long int);
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,const attachSystem::FixedComp&,
		     const long int, const long int);
  void createLinks();

 public:

  WedgeItem(const std::string&,const size_t);
  WedgeItem(const WedgeItem&);
  WedgeItem& operator=(const WedgeItem&);
  virtual ~WedgeItem();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int,
		 const long int);
  
};

}

#endif
 
