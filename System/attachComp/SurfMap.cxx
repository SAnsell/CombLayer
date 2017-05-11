/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/SurfMap.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Rules.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "SurfMap.h"

namespace attachSystem
{

SurfMap::SurfMap() : BaseMap()
 /*!
    Constructor 
  */
{}

SurfMap::SurfMap(const SurfMap& A) : 
  BaseMap(A)
  /*!
    Copy constructor
    \param A :: SurfMap to copy
  */
{}

SurfMap&
SurfMap::operator=(const SurfMap& A)
  /*!
    Assignment operator
    \param A :: SurfMap to copy
    \return *this
  */
{
  if (this!=&A)
    {
      BaseMap::operator=(A);
    }
  return *this;
}
  
HeadRule
SurfMap::getSurfRule(const std::string& Key,const size_t Index) const
  /*!
    Get the rule based on a surface
    \param Key :: Keyname
    \param Index :: Offset number
    \retun HeadRule
   */
{
  ELog::RegMethod RegA("SurfMap","getSurfRule(Key,index)"); 

  HeadRule Out;

  const int sn=(!Key.empty() && Key[0]=='-') ?
    -getItem(Key.substr(1),Index) : getItem(Key,Index);

  Out.addIntersection(sn);  
  return Out;
}

HeadRule
SurfMap::getSurfRules(const std::string& Key) const
  /*!
    Get the rule based on a surface
    \param Key :: Keyname
    \retun HeadRule
   */
{
  ELog::RegMethod RegA("SurfMap","getSurfRules(Key)"); 

  
  HeadRule Out;
  if (!Key.empty() && Key[0]=='-')
    {
      const std::vector<int> sVec=getItems(Key.substr(1));
      for(const int sn : sVec)
	Out.addUnion(-sn);
    }
  else
    {
      const std::vector<int> sVec=getItems(Key);

      for(const int sn : sVec)
	Out.addIntersection(sn);
    }
  return Out;
}



 
}  // NAMESPACE attachSystem
