/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testMatrix.h
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
#ifndef testMatrix_h
#define testMatrix_h 

/*!
  \class testMatrix
  \brief Test of Matrix class
  \version 1.0
  \date September 2005
  \author S.Ansell
  
  This needs some work to check various
  components on the matrix. Currently 
  a lot of info to std::cout but not 
  actually checking the results.
*/

class testMatrix 
{
private:

  //Tests 
  int testDiagonalise();
  int testDeterminate();
  int testEigenvectors();
  int testEigenvectors2();
  int testLaplaceDeterminate();
  int testSwapRows();
  int testInvert();

public:

  testMatrix();
  ~testMatrix();

  int applyTest(const int);     
};

#endif
