/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testMD5.h
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
/*
This file is part of KCTol

    KCTol is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with KCTol.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (C) Stuart Ansell 2001-2011. All rights reserved.

*/
#ifndef testMD5_h
#define testMD5_h 

/*!
  \class testMD5 
  \brief Test of MD5 sum code
  \version 1.0
  \date March 2011
  \author S.Ansell
*/

class testMD5 
{
private:

  //Tests 
  int testNext();

public:

  testMD5();
  ~testMD5();

  int applyTest(const int);     

};

#endif
