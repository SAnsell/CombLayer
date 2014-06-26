/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testFunction.h
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
#ifndef testFunction_h
#define testFunction_h 

/*!
  \class testFunction
  \brief Test the function parser
  \version 1.0
  \author S. Ansell
  \date April 2005
  
*/

class testFunction 
{
private:

  //Tests 
  int testAnalyse();
  int testBuiltIn();
  int testEval();
  int testString();
  int testVariable();
  int testVec3D();
  int testVec3DFunctions();
 
public:

  testFunction();
  ~testFunction();

  int applyTest(const int =0);     
};

#endif
