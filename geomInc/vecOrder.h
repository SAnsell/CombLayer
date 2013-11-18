/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/vecOrder.h
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
#ifndef Geometry_VecOrder_h
#define Geometry_VecOrder_h

namespace Geometry
{

/*!
  \class vecOrder 
  \brief compares Vec3D points based on x,y,z 
  \date August 2010
  \author S. Ansell
  \version 1.0
  
  This allows comparison of the vector for removing non-unique
  Vec3D from a list
*/

class vecOrder : public std::binary_function<Vec3D,Vec3D,bool>
{
 public:

  /// Implements a A<B
  bool operator()(const Vec3D& A,const Vec3D& B)
    {
      const double PTol(1e-8); 
      if (fabs(A.X()-B.X())>PTol)
	return (A.X()<B.X());
      if (fabs(A.Y()-B.Y())>PTol)
	return (A.Y()<B.Y());
      if (fabs(A.Z()-B.Z())>PTol)
	return (A.Z()<B.Z());
      return 0;
    }
};
  
}
#endif
