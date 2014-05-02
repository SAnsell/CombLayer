/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testDBMaterial.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef testDBMaterial_h
#define testDBMaterial_h 

/*!
  \class testDBMaterial
  \brief Tests the DBMaterial class
  \author S. Ansell
  \date April 2014
  \version 1.0
*/

class testDBMaterial 
{
private:

  //Tests 
  int testCombine();
 
public:

  testDBMaterial();
  ~testDBMaterial();

  int applyTest(const int);     

};

#endif
