/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testSVD.h
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
#ifndef testSVD_h
#define testSVD_h 

/*!
  \class testSVD
  \author S. Ansell
  \version 1.0
  \date October 2006
  \brief Test SVD for solving equations

*/

class testSVD
{
private:

  Geometry::SVD A;       ///< System for working with

  void init();

  //Tests 

  int testInit();
  int testMakeSolution();
  int testMakeSolLong();

public:

  testSVD();
  ~testSVD();

  int applyTest(const int);     
};


#endif
