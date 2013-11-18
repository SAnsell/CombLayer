/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testVec3D.h
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
#ifndef testVec3D_h
#define testVec3D_h 

/*!
  \class testVec3D 
  \brief Test class for the Vec3D class
  \version 1.0
  \date October 2007
  \author S.Ansell
  

*/

class testVec3D 
{
private:

  //Tests 
  int testDotProd();

public:

  testVec3D();
  ~testVec3D();

  int applyTest(const int extra =0);     
};

#endif
