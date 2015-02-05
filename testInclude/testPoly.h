/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testPoly.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef testPoly_h
#define testPoly_h 

/*!
  \class testPoly
  \brief Tests the Surface system
  \author S. Ansell
  \date October 2007
  \version 1.0

  Test all the polynominals.
*/

class testPoly 
{
private:

  //Tests 
  int testAddition();
  int testBase();
  int testBezout();
  int testBezoutFail();
  int testCopy();
  int testDurandKernerRoots();
  int testEqualTemplate();
  int testExpand();
  int testGetMaxSize();
  int testGetVarFlag();
  int testMinimalReduction();
  int testMultiplication();
  int testRead();
  int testSetComp();
  int testSingleVar();
  int testSubVariable();
  int testTriplet();
  int testVariable();
 
public:

  testPoly();
  ~testPoly();

  int applyTest(const int);
    
};

#endif
