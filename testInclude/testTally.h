/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testTally.h
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
#ifndef testTally_h
#define testTally_h 

/*!
  \class testTally
  \brief Tests the class Tally for subfunctions
  \author S. Ansell
  \date Nov. 2005
  \version 1.0

  Currently only tests the static Tally functions
*/

class testTally
{
private:

  //Tests 
  int testAddLine();
  int testAddParticles();
  int testFirstLine();
  int testFuCard();
  int testGetElm();
  int testGetID();


public:
  
  testTally();
  ~testTally();
  
  int applyTest(const int);       

};

#endif
