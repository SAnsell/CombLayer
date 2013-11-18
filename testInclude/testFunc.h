/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testFunc.h
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
#ifndef TestFunc_h
#define TestFunc_h

/*!
  \class TestFunc
  \brief Holds support functions for unit test
  \author S. Ansell
  \date April 2011
  \version 2.0
*/

class TestFunc
{
 private:

  int nTest;               ///< Number of tests
  int outStatus;           ///< Output status flag 

  std::string TGroup;      ///< Test Sector 
  std::string TSector;     ///< Test Sector 
  std::string TName;       ///< Test Name

  TestFunc();
  ///\cond NOT WRITTEN
  TestFunc(const TestFunc&);
  TestFunc& operator=(const TestFunc&);
  ///\endcond NOT WRITTEN
 
public:

  ~TestFunc();

  static TestFunc& Instance();
  static void regGroup(const std::string&);
  static void regSector(const std::string&);
  static void regTest(const std::string&);

  void setGroup(const std::string&);
  void setSector(const std::string&);
  void setTest(const std::string&);

  void regStatus(const int,const int,const int);

  void reportTest(std::ostream&) const;
  std::string summary() const;

  static void bracketTest(const std::string&,
			  std::ostream&);
  
};

#endif

