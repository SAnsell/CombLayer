/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   logInc/NameStack.h
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
#ifndef ELog_NameStack_h
#define ELog_NameStack_h

namespace ELog
{
  /*!
    \class NameStack 
    \brief Holds a list of items for a calling stack
    \author S. Ansell
    \version 1.0
    \date June 2009
  */
class NameStack
{
 private:

  std::map<std::string,int> key;        ///< Key names
  std::vector<std::string> Class;       ///< Class Name
  std::vector<std::string> Method;      ///< Method Name
  long int indentLevel;                 ///< Indent level

 public:

  NameStack();
  NameStack(const NameStack&);
  NameStack& operator=(const NameStack&);
  ~NameStack() {}    ///< Destructor

  void clear(); 
  
  void addComp(const std::string&,const std::string&);
  void popBack();

  std::string getBase() const;
  std::string getItem(const long int) const;
  std::string getFull() const;
  std::string getFullTree() const;

  /// Access depth of function:
  size_t getDepth() const { return Class.size(); }

  void addIndent(const long int);
  /// Output of the indent level
  long int indent() const { return indentLevel; }
  
};



}

#endif
