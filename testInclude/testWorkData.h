/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testWorkData.h
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
#ifndef testWorkData_h
#define testWorkData_h 

class WorkData;

/*!
  \class testWorkData
  \brief Tests the class Cylinder class
  \author S. Ansell
  \date Nov. 2005
  \version 1.0

  Test the distance of a point to the cone
*/

class testWorkData 
{
private:

  void populate(WorkData&,const int,const double,
		const double,const double,const double);

  //Tests 
  int testIntegral();
  int testSum();
 
public:

  testWorkData();
  ~testWorkData();

  int applyTest(const int);     
};

#endif
