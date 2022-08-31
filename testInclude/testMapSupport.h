/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testMapSupport.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef testMapSupport_h
#define testMapSupport_h 

/*!
  \class testMapSupport 
  \brief test of Map funtionals 
  \version 1.0
  \date September 2005
  \author S.Ansell
  
  This class currently only checks that 
  a command can be initialised, filled 
  and that results can be obtained.
*/

class testMapSupport 
{
private:

  //Tests 
  int testIterator();
  int testMapRange();
  int testValEqual();

public:

  testMapSupport();
  ~testMapSupport();

  int applyTest(const int =0);     
};

#endif
