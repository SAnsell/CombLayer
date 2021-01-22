/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testWriteSupport.h
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
#ifndef testWriteSupport_h
#define testWriteSupport_h 


/*!
  \class testWriteSupport 
  \brief test of Support components
  \version 1.0
  \date September 2005
  \author S.Ansell
  
  Checks the basic string operations in
  support.cxx and regexSupport.cxx
*/

class testWriteSupport
{
private:

  //Tests 
  int testDouble();
  int testPHITS();   

public:

  testWriteSupport();
  ~testWriteSupport();

  int applyTest(const int);     
};

#endif
