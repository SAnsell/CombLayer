/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   log/RegMethod.cxx
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
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "NameStack.h"
#include "RegMethod.h"

namespace ELog
{

NameStack RegMethod::Base;

RegMethod::RegMethod(const std::string& CN,
		     const std::string& MN) :
  indentLevel(0)
  /*!
    Constructor add name to stack
    \param CN :: Class name
    \param MN :: Method name
  */
{
  Base.addComp(CN,MN);
}

RegMethod::RegMethod(const std::string& CN,
		     const std::string& MN,
		     const int param) :
  indentLevel(0)
  /*!
    Constructor add name to stack
    \param CN :: Class name
    \param MN :: Method name
    \param param :: Index for type
  */
{
  std::ostringstream cx;
  cx<<"<"<<param<<">";
  Base.addComp(CN+cx.str(),MN);
}

RegMethod::~RegMethod() 
  /*!
    Destructor removes one from the stack
  */
{
  Base.popBack();
  if (indentLevel) 
    Base.addIndent(-indentLevel);
}

void
RegMethod::incIndent() 
  /*!
    Increase the indent level
  */
{
  indentLevel+=2;
  Base.addIndent(2);
  return;
}

void
RegMethod::decIndent() 
  /*!
    Increase the indent level
  */
{
  indentLevel-=2;
  Base.addIndent(-2);
  return;
}

} // NAMESPACE ELog
