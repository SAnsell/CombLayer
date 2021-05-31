/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupport/aType.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iterator>
#include <functional>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "phitsWriteSupport.h"
#include "aType.h"

namespace phitsSystem
{

aType::aType(const std::string& unitName,
	     const std::vector<double>& aPts) :
  aIndex(calcUnit(unitName)),
  aValue(aPts)
  /*!
    Constructor of vector of points
    \param aPts :: Bin edges in cos or deg.
  */
{
  ELog::RegMethod RegA("aType","aType(vector)");

  if (aValue.empty())
    ELog::EM<<"Error Angle Vector empty:"<<ELog::endErr;
}

aType::aType(const std::string& unitName,
	     const size_t NAngle,
	     const double minValue,
	     const double maxValue) :
  aIndex(calcUnit(unitName)*2),
  na(NAngle),aMin(minValue),aMax(maxValue)
  /*!
    Constructor linear or 
  */
{}

aType::aType(const aType& A) : 
  aIndex(A.aIndex),aValue(A.aValue),na(A.na),
  aMin(A.aMin),aMax(A.aMax)
  /*!
    Copy constructor
    \param A :: aType to copy
  */
{}

aType&
aType::operator=(const aType& A)
  /*!
    Assignment operator
    \param A :: aType to copy
    \return *this
  */
{
  if (this!=&A)
    {
      aIndex=A.aIndex;
      aValue=A.aValue;
      na=A.na;
      aMin=A.aMin;
      aMax=A.aMax;
    }
  return *this;
}

int
aType::calcUnit(const std::string& unitName) const
  /*!
    Set the unit from a name 
    \param unitName :: Name of the units
    \retval -1 : cos 
    \retval 1 : degree
   */
{
  const static std::map<std::string,int> unitMap
    ({
      {"degree",1},
      {"Degree",1},
      {"deg",1},
      {"Deg",1},
      {"cos",-1},
      {"Cos",-1}
    });

  std::map<std::string,int>::const_iterator mc=
    unitMap.find(unitName);
  if (mc==unitMap.end())
    throw ColErr::InContainerError<std::string>
      (unitName,"Atype::UnitName unknown");
  return mc->second;
}
  
void
aType::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("aType","write");

  
  StrFunc::writePHITS(OX,1,"a-type",aIndex);
  if (aIndex==1 || aIndex==-1)
    {
      std::string spc("    ");
      StrFunc::writePHITS(OX,2,"na",aValue.size()-1);

      int cnt(0);
      for(const double& AV : aValue)
	{
	  if (!(cnt % 4))
	    OX<<spc;
	  cnt++;
	  OX<<AV<<" ";
	}
      OX<<"\n";
    }
  else if (aIndex==2 || aIndex==-2)
    {
      StrFunc::writePHITS(OX,2,"na",na);
      StrFunc::writePHITS(OX,2,"amin",aMin);
      StrFunc::writePHITS(OX,2,"amax",aMax);
    }

  return;
}

}   // NAMESPACE phitsSystem
