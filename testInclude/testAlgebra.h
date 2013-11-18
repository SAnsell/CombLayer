/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testAlgebra.h
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
#ifndef testAlgebra_h
#define testAlgebra_h 

/*!
  \class testAlgebra
  \brief test Algebraic logic
  \author S. Ansell 
  \date August 2005
  \version 0.8
  
  Test the whole set of algebra object.
*/

class testAlgebra 
{
private:


  //Tests 
  int testAdditions();
  int testCNF();
  int testComplementary();
  int testCountLiterals();
  int testDNF();
  int testMakeString();
  int testMult();
  int testSetFunction();
  int testSetFunctionObjStr();
  int testSubtract();
  int testWeakDiv();     ///< test the weak division


public:

  testAlgebra();
  ~testAlgebra();

  int applyTest(const int);     
};

#endif
