/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystalInc/SymUnit.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef Crystal_SymUnit_h
#define Crystal_SymUnit_h

namespace Crystal
{

 /*!
   \class SymUnit
   \author S. Ansell
   \version 1.0
   \date March 2008
   \brief Holds cyrstal symmetry operation

   The symmetry operations are held by a vector
   group of three vectors
 */

class SymUnit
{
 private:

  Geometry::Vec3D OpSet[3];      ///< Operation list
  Geometry::Vec3D ShiftVec;      ///< Plus/Minus Half

 public:
  
  SymUnit();
  SymUnit& operator=(const SymUnit&);
  SymUnit(const SymUnit&);
  ~SymUnit();

  void setComponent(const size_t,const Geometry::Vec3D&);
  int setLine(const std::string&);

  Geometry::Vec3D unitPoint(const Geometry::Vec3D&) const;
  int symPoint(const Geometry::Vec3D&,Geometry::Vec3D&) const;

  std::string str() const;
  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const SymUnit&);

} // NAMESPACE Crystal


#endif
