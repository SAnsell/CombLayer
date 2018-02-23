/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/AcompTools.cxx
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
#include <climits>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "AcompTools.h"

namespace MonteCarlo 
{

namespace AcompTools
{
    

bool
unitsLessOrder(const int& A,const int& B)
  /*!
    Process the sorting of units
    \param A :: Unit to sort
    \param B :: Unit to sort		       
    
   */
{
  const int aPlus=std::abs(A);
  const int bPlus=std::abs(B);
  if (aPlus!=bPlus) return aPlus<bPlus;
  return (B<A);
}

void
unitSort(std::vector<int>& A)
  /*!
    Sort vector and remove duplicates
    \param A :: Vector to sort
  */
{
  std::sort(A.begin(),A.end(),unitsLessOrder);
  std::vector<int>::iterator vc=
    std::unique(A.begin(),A.end());
  A.erase(vc,A.end());
  return;
}
    
}  // NAMESPACE AcompTools

  
} // NAMESPACE MonteCarlo
