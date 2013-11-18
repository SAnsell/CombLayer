/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testSurfRegister.h
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
#ifndef testSurfRegister_h
#define testSurfRegister_h 

/*!
  \class testSurfRegister
  \brief Tests the surfRegister class
  \author S. Ansell
  \date Juley 2011
  \version 1.0
  
  Test of the surfRegister system, to avoid overlaps
  and for self-similar surfaces
*/

class testSurfRegister 
{
 private:
  
  //Tests 
  int testIdentical();
  int testPlaneReflection();
  int testUnique();
 
 public:

  testSurfRegister();
  ~testSurfRegister();

  int applyTest(const int);     
};

#endif
