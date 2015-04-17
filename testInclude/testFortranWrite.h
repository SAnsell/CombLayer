/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testFortranWrite.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef testFortranWrite_h
#define testFortranWrite_h 

/*!
  \class testFortranWrite
  \brief Tests the class FortranWrite
  \author S. Ansell
  \date April 2015
  \version 1.0
*/

class testFortranWrite
{
private:

  //Tests 
  int testParse();

public:
  
  testFortranWrite();
  ~testFortranWrite();
  
  int applyTest(const int);       

};

#endif
