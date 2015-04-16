/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   global/varNameOrder.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "varNameOrder.h"


varNameOrder::varNameOrder() 
  /*!
    Constructor
  */
{} 

bool 
varNameOrder::operator()(const std::string& A,
			 const std::string& B) const
  /*!
    Sort function that splits a name into (A number B)
    then sotrs of A B number 
    \param A :: First string
    \param B :: Second string
    \return A<B
  */
{
  size_t i=0;
  const size_t ABSize(std::max(A.size(),B.size()));
  while(i<ABSize && !std::isdigit(A[i]) &&
	A[i]==B[i])
    {
      i++;
    }
  if (i==ABSize || !std::isdigit(A[i]) || !std::isdigit(B[i]))
    return (A<B);

  std::string APart=A.substr(i);
  std::string BPart=B.substr(i);
  int aNum(0);
  int bNum(0);
  StrFunc::sectPartNum(APart,aNum);
  StrFunc::sectPartNum(BPart,bNum);
  if (APart!=BPart)
    return APart<BPart;

  return aNum<bNum;
}
