/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testMultiData.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef testMultiData_h
#define testMultiData_h 

/*!
  \class testMultiData 
  \brief Test class for the multiData layout
  \version 1.0
  \date October 2023
  \author S.Ansell
  

*/

class testMultiData 
{
private:

  //Tests 
  int testExchange();
  int testGetAxisRange();
  int testIntegrateMap();
  int testIntegrateMapRange();
  int testIntegrateValue();
  int testMultiRange();
  int testProjectMap();
  int testProjectMapDebug();
  int testRange();
  int testSet();
  
public:

  testMultiData();
  ~testMultiData();

  int applyTest(const int extra);
  
};

#endif
