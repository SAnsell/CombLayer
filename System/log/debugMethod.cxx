/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   log/debugMethod.cxx
*
 * Copyright (c) 2004-2015 by Stuart Ansell
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

#include "Debug.h"
#include "debugMethod.h"

namespace ELog
{

std::vector<int> debugMethod::ABase;  
int debugMethod::debugCnt(0);

debugMethod::debugMethod()
  /*!
    Constructor add name to stack
  */
{
  ABase.push_back(0);
}

void
debugMethod::activate()
  /*!
    Activate the debug 
  */
{
  if (!ABase.back())
    {
      ABase.back()=1;
      debugStatus::Instance().setFlag(1);
      debugCnt++;
    }
  return;
}

void
debugMethod::deactivate()
  /*!
    Deactivate the debug 
  */
{
  if (ABase.back()==1)
    {
      ABase.back()=0;
      debugCnt--;
      if (!debugCnt)
	debugStatus::Instance().setFlag(0);
    }
  return;
}

debugMethod::~debugMethod() 
  /*!
    Destructor removes one from the stack
  */
{
  deactivate();
}

} // NAMESPACE ELog
