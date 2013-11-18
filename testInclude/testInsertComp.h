/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testInsertComp.h
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
#ifndef testInsertComp_h
#define testInsertComp_h 

/*!
  \class testInsertComp
  \brief Tests Inserting rules
  \author S. Ansell
  \date August 2010
  \version 1.0
  
  Test the insert system
*/


class testInsertComp 
{
private:


  //Tests 
  int testAddition();
 
public:

  testInsertComp();
  ~testInsertComp();

  int applyTest(const int);     
};

#endif
