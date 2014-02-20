/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testUnitSupport.h
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
#ifndef UnitTest_testUnitSupport_h 
#define UnitTest_testUnitSupport_h 

/*!
  \namespace UnitTest
  \author S. Ansell
  \version 1.0
  \date June 2006
  \brief Holds classes used solely for testing
*/

namespace UnitTest
{

/*!
  \class Lower
  \brief Class at base of container list
  \author S. Ansell
  \version 1.0
  \date August 2007
 */

class Lower
{
 public:
  
  int a;   ///< Paramerter to set/access
  int b;   ///< Paramerter to set/access
  int c;   ///< Paramerter to set/access
  
  /// Constructor
  Lower(const int x) : a(x),b(x),c(x) {}

  void procXML(XML::XMLcollect&) const;

  void write() const;
};


/*!
  \class Mid
  \brief Class at middle of container list
  \author S. Ansell
  \version 1.0
  \date August 2007
 */
class Mid
{
 public:

  int a;            ///< Paramerter to set/access
  int b;            ///< Paramerter to set/access
  Lower Hold;       ///< Lower class container

  /// Constructor
  Mid(const int x) : a(x),b(x),Hold(x-2) {}
  void procXML(XML::XMLcollect&) const;
  void write() const;

};

/*!
  \class Top
  \brief Class at top of container list
  \author S. Ansell
  \version 1.0
  \date August 2007
 */
class Top 
{
 public:

  int a;         ///< Parameter to set
  int b;         ///< Parameter to set
  Mid Hold;      ///< Mid class container

  /// Constructor
  Top(const int x) : a(x),b(x-10),Hold(x-3) {} 

  void procXML(XML::XMLcollect&) const;
  void write() const;

};


}   // NAMESPACE UnitTest

#endif
