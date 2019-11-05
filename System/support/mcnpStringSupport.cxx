/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/mcnpStringSupport.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <string>
#include <iterator>
#include <algorithm>


#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "support.h"
#include "mcnpStringSupport.h"

/*! 
  \file mcnpStringSupport.cxx 
*/

namespace mcnpFunc
{

bool
keyUnit(std::string& Ln,std::string& Key,std::string& value)
/*!
    Given a string extract the first XXX = YYY parts
    \param Ln :: Line to extract and cut
    \param Key :: Key component [left side/xxx]
    \param value :: Value component [right side/yyy]
    \return true if unit found.
   */
{
  std::string::size_type posA=Ln.find('=');
  std::string::size_type posB(posA);
  if (posA==std::string::npos)
    return 0;

  Key.clear();
  int spc(0);
  for(;posA!=0 && spc!=2;posA--)
    {
      if (isspace(Ln[posA-1]))
	spc=(Key.empty()) ? 1 : 2;
      else
	Key+=Ln[posA-1];
    }
  if (Key.empty())
    return 0;
  std::reverse(Key.begin(),Key.end());
  std::transform(Key.begin(),Key.end(),
		 Key.begin(),::tolower);
  if (spc==2) posA++;
  
  value.clear();
  spc=0;
  for(posB++;posB!=Ln.size() && spc!=2;posB++)
    {
      if (isspace(Ln[posB]))
	spc=(value.empty()) ? 1 : 2;
      else
	value+=Ln[posB];
    }
  if (value.empty())
    return 0;

  Ln.erase(posA,posB-posA);  
  return 1;
}

bool
startMaterial(const std::string& Line) 
  /*!
    Static object to deterimine if the
    input string 'Line' starts an object 
    \param Line :: Object to test
    \retval 1 :: Start
    \retval 0 :: Not the start of aline
  */
{
  int n,matN;
  double rho;
  // Need line of type id matNumber
  std::string Tst=Line;
  if (StrFunc::section(Tst,n) && n>0 &&           // must start id + matN
      StrFunc::section(Tst,matN) && matN>=0)      
    {
      if (matN==0)
	return 1;

      std::string A;
      if (StrFunc::section(Tst,A) &&  StrFunc::convert(A,rho))
	{
	  if (A.find('.')!=std::string::npos &&    // avoid problem of 0.0
	      rho<10.0)
	    return 1;
	}
    }

  return 0;         
}

}
