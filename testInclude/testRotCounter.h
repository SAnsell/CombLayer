/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testRotCounter.h
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
#ifndef testRotCounter_h
#define testRotCounter_h 

/*!
  \class testRotCounter
  \brief Tests the limited range counter
  \author S. Ansell
  \date Nov. 2005
  \version 1.0
  
  Test the ability to go 1.2.3.4.0.1 etc 
*/

class testRotCounter 
{
private:


  //Tests 
  int testAddition();
 
public:

  testRotCounter();
  ~testRotCounter();

  int applyTest(const int);     
};

#endif
