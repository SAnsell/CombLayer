/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   logInc/RegMethod.h
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
#ifndef ELog_RegMethod_h
#define ELog_RegMethod_h

namespace ELog
{
  /*!
    \class RegMethod 
    \brief Holds a list of items for a calling stack
    \author S. Ansell
    \date June 2009
    \version 1.0

    This class is called as a registration class.
    It keeps location etc possible for 
  */

class RegMethod
{
 private:

  static NameStack Base;           ///< Singleton of base to register

  int indentLevel;                 ///< Additional indent
  /// \cond NOWRITTEN
  RegMethod(const RegMethod&);
  RegMethod& operator=(const RegMethod&);
  /// \endcond NOWRITTEN

 public:

  /// Access NameStack pointer
  NameStack* getBasePtr() { return &Base; }
  RegMethod(const std::string&,const std::string&);
  RegMethod(const std::string&,const std::string&,const int);
  ~RegMethod();

  /// Access string
  static std::string getBase() { return Base.getBase(); }
  /// Access string
  static std::string getFull() { return Base.getFullTree(); }
  /// Access particular item 
  static std::string getItem(const int I) { return Base.getItem(I); }

  void incIndent();
  void decIndent();

};

}

#endif
