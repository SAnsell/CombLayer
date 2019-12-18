/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/SurfMap.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Surface.h"
#include "surfRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "surfIndex.h"
#include "SurInter.h"
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
SurfMap::getSignedSurf(const std::string& Key,
		       const size_t Index) const
  /*!
    Get the rule based on a surface
    \param Key :: Keyname
    \param Index :: Offset number
    \return Signed surface map
   */
{
  ELog::RegMethod RegA("SurfMap","getSignedSurf");

  if (Key.empty()) return 0;

  return (Key[0]=='-' || Key[0]=='#' || 
	  Key[0]=='*' || Key[0]=='%') ?
    -getItem(Key.substr(1),Index) : getItem(Key,Index);
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

  if (!Key.empty())
    {
      const int sn=(Key[0]=='-' || Key[0]=='#' || Key[0]=='*') ?
	-getItem(Key.substr(1),Index) : getItem(Key,Index);
      
      Out.addIntersection(sn);
    }
  return Out;
}

Geometry::Surface*
SurfMap::getSurfPtr(const std::string& Key,
		    const size_t Index) const
  /*!
    Get the rule based on a surface
    \param Key :: Keyname
    \param Index :: Index number
    \return surface pointer
   */
{
  ELog::RegMethod RegA("SurfMap","getSurfPtr");
  // care hear this should not be singleton
  const ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();

  if (Key.empty()) return 0;
  const int sn=getSignedSurf(Key,Index);
  return (sn<0) ?  SurI.getSurf(-sn) : SurI.getSurf(sn);

}

template<typename T>
T*
SurfMap::realPtr(const std::string& Key,
		 const size_t Index) const
  /*!
    Get the rule based on a surface
    \tparam T :: Surf type						
    \param Key :: Keyname
    \param Index :: Index number
    \return surface pointer as type T
   */
{
  ELog::RegMethod RegA("SurfMap","realPtr");
  // care hear this should not be singleton
  const ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();

  if (Key.empty()) return 0;

  const int sn=getSignedSurf(Key,Index);
  return SurI.realSurf<T>(std::abs(sn));
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
SurfMap::getSurfStringOrNumber(const std::string& Key) const
  /*!
    Output the rule string
    \param Key :: Surf Keyname
    \return surfaces
  */
{
  ELog::RegMethod RegA("SurfMap","getSurfStringOrNumber");

  // if valid number return number
  int N;
  if (StrFunc::convert(Key,N)) return Key;
  return getSurfRules(Key).display();
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
SurfMap::getSurfComplementOrNumber(const std::string& Key) const
  /*!
    Output the rule string [complement]
    \param Key :: Surf Keyname
    \return surfaces
  */
{
  ELog::RegMethod RegA("SurfMap","getSurfComplementOrNumber");
  // if valid number return number
  int N;
  if (StrFunc::convert(Key,N))
    return std::to_string(-N);
  
  return getSurfRules(Key).complement().display();
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
SurfMap::combine(const std::string& surfStr) const
  /*!
    Add the rules as intesection
    \param surfStr :: Keynames of surfaces (space sparated)
    \return HeadRule [form: s1 s2 s3 ]
    \todo make work with full string objects
   */
{
  ELog::RegMethod RegA("SurfMap","combine(string)"); 

  std::string HR;
  std::string part;
  bool complement(0);
  for(const char C : surfStr)
    {
      if (std::isdigit(static_cast<int>(C)) || 
	  std::isalpha(static_cast<int>(C)))
	part+=C;
      else if (C=='-')
	complement=1;
      else if (!part.empty())
	{
	  HR+=(complement) ? getSurfComplementOrNumber(part) :
	    getSurfStringOrNumber(part);
	  HR+=" ";
	  part="";
	  complement=0;
	}
      else
	HR+=C;
    }
  if (!part.empty())
    {
      HR+=(complement) ? getSurfComplementOrNumber(part) :
	getSurfStringOrNumber(part);
    }
  return HeadRule(HR);
}

void
SurfMap::createLink(const std::string& surfName,
		    attachSystem::FixedComp& FC,
		    const size_t linkIndex,
		    const Geometry::Vec3D& Org,
		    const Geometry::Vec3D& YAxis) const
  /*!
    Generate the line link from the origin along YAxis
    \param extName :: Cut Unit item
    \param FC :: Fixed component [most likely this]
    \param linkIndex :: link point to build
    \param Org :: Origin
    \param YAxis :: YAxis
   */
{
  ELog::RegMethod RegA("SurfMap","createLinks");

  const Geometry::Surface* SPtr=getSurfPtr(surfName);

  if (!SPtr)
    throw ColErr::InContainerError<std::string>
      (surfName,"Surface not found");

  FC.setLinkSurf(linkIndex,getSignedSurf(surfName));
  FC.setConnect(linkIndex,SurInter::getLinePoint(Org,YAxis,SPtr,Org),YAxis);

  return;
}


///\cond template
template Geometry::Plane*
SurfMap::realPtr(const std::string&,const size_t) const;
  
///\endcond template
  
  
}  // NAMESPACE attachSystem
