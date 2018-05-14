/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testSurfImplicate.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef testSurfImplicate_h
#define testSurfImplicate_h 

/*!
  \class testSurfImplicate
  \brief Tests the class Plane class
  \author S. Ansell
  \date March 2018
  \version 1.0

  Test the implicates on surf-surf
*/

class testSurfImplicate 
{
private:

  //Tests 
  int testPlanePlane();
  int testPlaneCylinder();
 
public:

  testSurfImplicate();
  ~testSurfImplicate();

  int applyTest(const int);     
};

#endif
