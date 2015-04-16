/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   visitInc/MatMD5.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef MatMD5_h
#define MatMD5_h

/*!
  \class MatMD5
  \author S. Ansell
  \version 1.0
  \date March 2011
  \brief Summary of a material selection
*/

class MatMD5
{
 private:
  
  long int N;                    ///< Number of components
  Geometry::Vec3D sumXYZ;        ///< sum of Vector
  Geometry::Vec3D sqrXYZ;        ///< sum*sum of vector
  
 public:
  
  MatMD5();
  MatMD5(const MatMD5&);
  MatMD5& operator=(const MatMD5&);
  ~MatMD5();
  
  /// Have points been added
  bool isEmpty() const { return (N) ? 0 : 1; }

  void addUnit(const Geometry::Vec3D&);
  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const MatMD5&);



#endif
