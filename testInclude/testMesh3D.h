/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testMesh3D.h
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
#ifndef testMesh3D_h
#define testMesh3D_h 

namespace Geometry
{
  class Mesh3D;
}

/*!
  \class testMesh3D
  \brief Tests the class Cylinder class
  \author S. Ansell
  \date Nov. 2005
  \version 1.0

  Test the distance of a point to the cone
*/

class testMesh3D 
{
private:


  void createXYZ(Geometry::Mesh3D&,
		 const std::string&,const std::string&,
		 const std::string&) const;
  //Tests 
  int testPoint();
 
public:

  testMesh3D();
  ~testMesh3D();

  int applyTest(const int);     
};

#endif
