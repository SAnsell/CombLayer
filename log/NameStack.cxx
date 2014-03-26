/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   log/NameStack.cxx
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
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "NameStack.h"

namespace ELog
{

NameStack::NameStack() :
  indentLevel(0)
  /*!
    Constructor
  */
{}

NameStack::NameStack(const NameStack& A) :
  key(A.key),Class(A.Class),Method(A.Method),
  indentLevel(A.indentLevel)
  /*!
    Copy Constructor
    \param A :: NameStack to copy
  */
{}

NameStack&
NameStack::operator=(const NameStack& A) 
  /*!
    Assignment operator
    \param A :: NameStack to copy
    \return *this
  */
{
  if (this!=&A)
    {
      key=A.key;
      Class=A.Class;
      Method=A.Method;
      indentLevel=A.indentLevel;
    }
  return *this;
}

void
NameStack::clear()
 /*!
   Clear the stack
 */
{
  key.erase(key.begin(),key.end());
  Class.clear();
  Method.clear();
  indentLevel=0;
  return;
}

void
NameStack::addComp(const std::string& CN,
		   const std::string& MN)
  /*!
    Adds a component to the class names series
    \param CN :: Class name
    \param MN :: Method name
  */
{
  Class.push_back(CN);
  Method.push_back(MN);
  return;
}

void
NameStack::popBack() 
  /*!
    Pop back a item
  */
{
  if (!Class.empty())
    {
      Class.pop_back();
      Method.pop_back();
    }
  return;
}

std::string
NameStack::getBase() const 
  /*!
    Return the base component
    \return BaseItem
  */
{
  return (Class.empty()) ?  
    "" : Class.back()+"::"+Method.back();
}

std::string
NameStack::getItem(const long int Index) const 
  /*!
    Return the base component. If Index is -ve then
    it is relative to the end of the size.
    \param Index :: Item 
    \return BaseItem
  */
{
  if (Class.empty()) return "";
  if (!Index) 
    return Class.back()+"::"+Method.back();
  
  const size_t CSize=Class.size();
 
  const size_t itx( (Index<0) 
		    ? (CSize-static_cast<size_t>(1-Index)) 
		    : static_cast<size_t>(Index));

  return (itx<CSize) ? Class[itx]+"::"+Method[itx] : "";
} 

std::string
NameStack::getFull() const 
  /*!
    Return the base component
    \return BaseItem
  */
{
  std::vector<std::string>::const_iterator vc(Class.begin());
  std::vector<std::string>::const_iterator ac(Method.begin());
  std::string Out=*vc+"::"+*ac;
  for(ac++,vc++;vc!=Class.end();vc++,ac++)
    {
      Out+="#";
      Out+=*vc+"::"+*ac;
    }
  return Out;
}

std::string
NameStack::getFullTree() const 
  /*!
    Return the base component
    \return BaseItem
  */
{
  std::vector<std::string>::const_iterator vc(Class.begin());
  std::vector<std::string>::const_iterator ac(Method.begin());
  size_t indent(2);
  std::string Out=*vc+"::"+*ac;
  for(ac++,vc++;vc!=Class.end();vc++,ac++,indent+=2)
    {
      Out+="\n";
      Out+=std::string(indent,' ');
      Out+=*vc+"::"+*ac;
    }
  return Out;
}

void
NameStack::addIndent(const long int N) 
  /*!
    Add a value to the indent level.
    Forces 0 and positive numbers.
    \param N :: Number to add
  */
{
  indentLevel+=N;
  if (indentLevel<0)
    indentLevel=0;
  return;
}

} // NameSpace ELog
