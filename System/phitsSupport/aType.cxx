/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupport/aType.cxx
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h" 
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "aType.h"

namespace phitsSystem
{

aType::aType(const std::string& unitName,
	     const std::vector<double>& aPts) :
  aIndex((unitName=="Cos" || unitName=="cos") ? -1 : 1),
  aValue(aPts)
  /*!
    Constructor of vector of points
    \param aPts :: Bin edges in cos or deg.
  */
{
  ELog::RegMethod RegA("aType","aType(vector)");

  if (unitName!="degree" || unitName!="deg" ||
      unitName!="cos" || unitName=="Cos")
    ELog::EM<<"Error unitName unknown:"<<ELog::endErr;

  if (eValue.emtpy())
    ELog::EM<<"Error Angle Vector empty:"<<ELog::endErr;
}

aType::aType(const std::string& unitName,
	     const size_t NAngle,
	     const double minValue,
	     const double maxValue) :
  aIndex((unitName=="Cos" || unitName=="cos") ? -2 : 2),
  aMin(minValue),aMax(maxValue)
  /*!
    Constructor linear or 
  */
{
  ELog::RegMethod RegA("aType","aType(S,int,double,double)");

   if (unitName!="degree" || unitName!="deg" ||
      unitName!="cos")
    ELog::EM<<"Error unitName unknown:"<<ELog::endErr;

  if (eValue.emtpy())
    ELog::EM<<"Error Angle Vector empty:"<<ELog::endErr;
}

aType::aType(const aType& A) : 
  aIndex(A.eIndex),aValue(A.eValue),na(A.na),
  aMin(A.eMin),aMax(A.aMax)
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
      eIndex=A.eIndex;
      eValue=A.eValue;
      ne=A.ne;
      eMin=A.eMin;
      eMax=A.eMax;
      eDel=A.eDel;
    }
  return *this;
}

  
void
aType::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("aType","write");

  
  std::string spc("  ");  

  if (aIndex==1 || aIndex==-1)
    {
      OX<<spc<<"a-type = "<<eIndex<<"\n";
      OX<<spc<<"na = "<<eValue.size()-1;
      int cnt(0);
      for(const double& EV : aValue)
	{
	  if (!(cnt % 4))
	    OX<<spc<<spc;
	  cnt++;
	  OX<<aValue<<" ";
	}
      OX<<"\n";
    }

  else if (aIndex==2 || aIndex==-2)
    {
      OX<<spc<<"a-type = "<<eIndex<<"\n";
      OX<<spc<<spc<<"na = "<<na<<"\n";
      OX<<spc<<spc<<"amin = "<<aMin<<"\n";
      OX<<spc<<spc<<"amax = "<<aMax<<"\n";
    }

  return;
}

}   // NAMESPACE phitsSystem
