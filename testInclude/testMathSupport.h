/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testMathSupport.h
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
#ifndef testMathSupport_h
#define testMathSupport_h 

/*!
  \class testMathSupport 
  \brief test of mathSupport objects
  \version 1.1
  \date January 2006
  \author S.Ansell
  
  This class currently only checks the
  - Order function and PairSort class
  - crossOrder and PCombine class
*/

class testMathSupport 
{
private:

  int validCubic(const double*,const std::complex<double>&) const;

  //Tests 
  int testBinSearch();
  int testClebschGordan();
  int testCountBits();
  int testCubic();
  int testFibinacci();
  int testIndexSort();
  int testLowBitIndex();
  int testMinDifference();  
  int testNormalDist();  
  int testOrder();
  int testPairCombine();   
  int testPairSort();   
  int testPolInterp();   
  int testQuadratic();

public:

  testMathSupport();
  ~testMathSupport();

  int applyTest(const int =0);     
};

#endif
