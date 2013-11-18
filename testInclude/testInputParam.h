/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testInputParam.h
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
#ifndef testInputParam_h
#define testInputParam_h 

/*!
  \class testInputParam
  \brief InputParam insert/find/setting
  \author S. Ansell
  \date April 2011
  \version 1.0
  
  Test the insert system
*/


class testInputParam 
{
private:

  static void initInput(const std::string[],std::vector<std::string>&);

  //Tests 
  int testDefValue();
  int testFlagDef();
  int testGetValue();
  int testInput();
  int testMulti();
  int testMultiExtract();
  int testMultiTail();
  int testSetValue();
  int testWrite();
  int testWriteDesc();
 
public:

  testInputParam();
  ~testInputParam();

  int applyTest(const int);     
};

#endif
