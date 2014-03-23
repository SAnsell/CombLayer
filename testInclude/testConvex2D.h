/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testConvex2D.h
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
#ifndef testConvex2D_h
#define testConvex2D_h 

/*!
  \class testConvex2D
  \brief Tests the class Cone class
  \author S. Ansell
  \date Nov. 2005
  \version 1.0

  Test the distance of a point to the cone
*/

class testConvex2D 
{
private:

  Geometry::Convex2D A;      ///< Cone object to create/work on 

  static void initConvexPlane(Geometry::Convex2D&,const Geometry::Vec3D&,
			      const Geometry::Vec3D&,const int,
			      const double,const double);
  static void initCircle(Geometry::Convex2D&,const Geometry::Vec3D&,
			 const Geometry::Vec3D&,const double,const int,const double);

  //Tests 
  int testArea();
  int testInHull();
  int testMaxElement();
  int testHull();
  int testRandomPlane();
  int testSimplePlane();

public:

  testConvex2D();
  ~testConvex2D();

  int applyTest(const int);     
};

#endif
