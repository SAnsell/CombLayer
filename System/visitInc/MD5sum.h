/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   visitInc/MD5sum.h
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
#ifndef MD5sum_h
#define MD5sum_h

class Simulation;
namespace MonteCarlo
{
  class Object;
}



/*!
  \class MD5sum
  \brief Calculate the MD5 sum of a model
  \date August 2010
  \author S. Ansell
  \version 1.0
*/
						
class MD5sum
{
 private:
  
  // Input data
  Geometry::Vec3D Origin;     ///< Origin
  Geometry::Vec3D XYZ;        ///< XYZ extent
  Triple<size_t> nPts;        ///< Number x points
  
  /// Calc results:
  std::vector<MatMD5> Results;

 public:

  MD5sum(const size_t);
  MD5sum(const MD5sum&);
  MD5sum& operator=(const MD5sum&);
  ~MD5sum();

  void setBox(const Geometry::Vec3D&,
              const Geometry::Vec3D&);
  void setIndex(const size_t,const size_t,const size_t);

  void populate(const Simulation*);
  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const MD5sum&);

#endif
