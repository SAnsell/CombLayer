/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testHeadRule.h
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
#ifndef testHeadRule_h
#define testHeadRule_h 

/*!
  \class testHeadRule
  \brief Tests Rule holder for surface objects.
  \author S. Ansell
  \date July 2014
  \version 1.0  
*/


class testHeadRule
{
private:

  void createSurfaces();

  //Tests 
  int testAddInterUnion();
  int testCountLevel();
  int testEqual();
  int testFindNodes();
  int testFindTopNodes();
  int testGetComponent();
  int testInterceptRule();
  int testLevel();
  int testPartEqual();
  int testRemoveSurf();
  int testReplacePart();
  int testSurfSet();
 
public:

  testHeadRule();
  ~testHeadRule();

  int applyTest(const int);     
};

#endif
