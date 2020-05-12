/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testObjectRegister.h
*
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef testObjectRegister_h
#define testObjectRegister_h 

/*!
  \class testObjectRegister
  \brief Tests the class Object
  \author S. Ansell
  \date October 2012
  \version 1.0

  Test the processing of object 
*/

class testObjectRegister
{
private:
  
  //Tests 
  int testExcludeItem();

public:
  
  testObjectRegister();
  ~testObjectRegister();
  
  int applyTest(const int);       

};

#endif
