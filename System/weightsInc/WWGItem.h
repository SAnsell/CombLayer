/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightInc/WWGItem.h
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
#ifndef WeightSystem_WWGItem_h
#define WeightSystem_WWGItem_h

namespace WeightSystem
{

/*!
  \class WWGItem 
  \version 1.0
  \author S. Ansell
  \date November 2016
  \brief Point weights in WWG mesh
*/

class WWGItem
{
 public:

  const Geometry::Vec3D Pt;     ///< Centre point
  std::vector<double> weight;          ///< weight for the cell[eneryg]

  explicit WWGItem(const Geometry::Vec3D&);

  WWGItem(const WWGItem&);
  WWGItem& operator=(const WWGItem&);
  ~WWGItem() {}  ///< Destructor

  void write(std::ostream&) const;
};

}

#endif
