/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testNRange.h
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
#ifndef testNRange_h
#define testNRange_h 

/*!
  \class testNRange
  \brief Tests the class NRange for intervals 
  \author S. Ansell
  \date Nov. 2005
  \version 1.0

  Test the processing of ranges X log Y and X NI Y
*/

class testNRange
{
private:

  //Tests 
  int testCondense();
  int testOperator();
  int testOutput();
  int testRange();

public:
  
  testNRange();
  ~testNRange();
  
  int applyTest(const int);       

};

#endif
