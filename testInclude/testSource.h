/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testSource.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef testSource_h
#define testSource_h 

/*!
  \class testSource
  \brief Tests the Source objects
  \author S. Ansell
  \date July 2009
  \version 1.0
  
  Test Source output
*/

class testSource 
{
private:

  static int checkSDef(const std::string&,const std::string&);
  //Tests 
  int testBasic();
  int testItem();
  int testProbTable();
 
public:

  testSource();
  ~testSource();

  int applyTest(const int);     
};

#endif
