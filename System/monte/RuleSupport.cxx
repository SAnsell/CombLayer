/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/RuleSupport.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <complex>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include <iterator>

#include "FileReport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "MatrixBase.h"
#include "HeadRule.h"

namespace MonteCarlo
{

std::string
getComplementShape(const std::string& innerUnit)
  /*!
    Process inner string to give complement  
    \param extraUnion :: inner part
    \return string of outer part
   */
{
  ELog::RegMethod RegA("RuleSupport[F]","getExcludeShape");
  HeadRule A;
  A.procString(innerUnit);
  A.makeComplement();
  return A.display();
}

}  // NAMESPACE MonteCarlo
