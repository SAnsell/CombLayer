/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testM3.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef testM3_h
#define testM3_h 

/*!
  \class testM3 
  \brief Test class for the multiData layout
  \version 1.0
  \date October 2023
  \author S.Ansell
  

*/

class testM3 
{
private:

  //Tests 
  int testDiagonalize();
  int testEchelon();

 public:

  testM3();
  ~testM3();

  int applyTest(const int extra);
  
};

#endif
