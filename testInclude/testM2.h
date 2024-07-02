/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testM2.h
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
#ifndef testM2_h
#define testM2_h 

/*!
  \class testM2 
  \brief Test class for the multiData layout
  \version 1.0
  \date October 2023
  \author S.Ansell
  

*/

class testM2 
{
private:

  //Tests 
  int testInit();
  int testEigenValues();

 public:

  testM2();
  ~testM2();

  int applyTest(const int extra);
  
};

#endif
