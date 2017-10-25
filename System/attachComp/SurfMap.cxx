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

int
SurfMap::getSignedSurf(const std::string& K,const long int Index) const
  /*!
    Get the rule based on a surface
    \param K :: Keyname
    \param Index :: Offset number [signed]
    \return Signed surface map
   */
{
  ELog::RegMethod RegA("SurfMap","getSignedSurf");

  if (!Index)
    return BaseMap::getItem(K);
  else if (Index>0)
    return BaseMap::getItem(K,static_cast<size_t>(Index-1));
  else
    return -BaseMap::getItem(K,static_cast<size_t>(-Index-1));
}
  
HeadRule
SurfMap::getSurfRule(const std::string& Key,const size_t Index) const
  /*!
    Get the rule based on a surface
    \param Key :: Keyname
    \param Index :: Offset number
    \return HeadRule
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
    \return HeadRule
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

std::string
SurfMap::getSurfString(const std::string& Key) const
  /*!
    Output the rule string
    \param Key :: Surf Keyname
    \return surfaces
  */
{
  ELog::RegMethod RegA("SurfMap","getSurfString");

  return getSurfRules(Key).display();
}

std::string
SurfMap::getSurfComplement(const std::string& Key) const
  /*!
    Output the rule string [complement]
    \param Key :: Surf Keyname
    \return surfaces
  */
{
  ELog::RegMethod RegA("SurfMap","getSurfComplement");

  return getSurfRules(Key).complement().display();
}

  
HeadRule
SurfMap::combine(const std::set<std::string>& KeySet) const
  /*!
    Add the rules as intesection
    \param KeySet :: Keynames
    \return HeadRule
   */
{
  ELog::RegMethod RegA("SurfMap","combine"); 
  
  HeadRule Out;
  for(const std::string& KS : KeySet)
    Out.addIntersection(getSurfRules(KS));

  return Out;
}



 
}  // NAMESPACE attachSystem
