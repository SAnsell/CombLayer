/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupport/eType.cxx
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
#include "eType.h"

namespace phitsSystem
{

eType::eType(const std::vector<double>& ePts) :
  eIndex(1),
  eValue(ePts)
  /*!
    Constructor of vector of points
    \param ePts :: Energy points
  */
{
  ELog::RegMethod RegA("eType","eType(vector)");
  
  if (eValue.empty())
    ELog::EM<<"Error Energy Vector empty:"<<ELog::endErr;
}

eType::eType(const std::string& tName,
	     const size_t NEnergy,
	     const double minValue,
	     const double maxValue) :
  eIndex((tName=="Log" || tName=="log") ? 3 : 2),
  ne(NEnergy),eMin(minValue),eMax(maxValue)
  /*!
    Constructor linear or log
    \param tName :: Data type [log/linear]
  */
{
  ELog::RegMethod RegA("eType","eType(S,int,double,double)");
  
  if (tName!="Linear" || tName!="Log" ||
      tName!="linear" || tName!="log")
    ELog::EM<<"Error e-type unknown:"<<tName<<ELog::endErr;

  if (eMin>eMax)
    ELog::EM<<"Error Energy-Range -ve:"<<eMin<<" "<<eMax<<ELog::endErr;
  
}

eType::eType(const std::string& tName,
	     const double minValue,
	     const double maxValue,
	     const double delta) :
  eIndex((tName=="Log" || tName=="log") ? 5 : 4),
  eMin(minValue),
  eMax(maxValue),
  eDel(delta)
  /*!
    Constructor linear or 
  */
{
  ELog::RegMethod RegA("eType","eType(S,double,double,double)");
  
  if (tName!="Linear" || tName!="Log" ||
      tName!="linear" || tName!="log")
    ELog::EM<<"Error e-type unknown:"<<tName<<ELog::endErr;

  if (minValue>maxValue)
    ELog::EM<<"Error Energy-Range -ve:"<<minValue<<" "<<maxValue<<ELog::endErr;

  if (eDel<0)
    ELog::EM<<"Error Energy Delta :"<<eDel<<ELog::endErr;
  
}




  
eType::eType(const eType& A) : 
  eIndex(A.eIndex),eValue(A.eValue),ne(A.ne),
  eMin(A.eMin),eMax(A.eMax),eDel(A.eDel)
  /*!
    Copy constructor
    \param A :: eType to copy
  */
{}

eType&
eType::operator=(const eType& A)
  /*!
    Assignment operator
    \param A :: eType to copy
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
eType::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("eType","write");

  
  std::string spc("  ");
  

  if (eIndex==1)
    {
      OX<<spc<<"e-type = "<<eIndex;
      OX<<spc<<"ne     = "<<eValue.size()-1;
      int cnt(0);
      for(const double EV : eValue)
	{
	  if (!(cnt % 4))
	    OX<<spc<<spc;
	  cnt++;
	  OX<<EV<<" ";
	}
      OX<<"\n";
    }

  else if (eIndex==2 || eIndex==3)
    {
      OX<<spc<<"e-type   = "<<eIndex<<"\n";
      OX<<spc<<spc<<"ne   = "<<ne<<"\n";
      OX<<spc<<spc<<"emin = "<<eMin<<"\n";
      OX<<spc<<spc<<"emax = "<<eMax<<"\n";
    }
  else if (eIndex==4 || eIndex==5)
    {
      OX<<spc<<"e-type = "<<eIndex<<"\n";
      OX<<spc<<spc<<"edel = "<<eDel<<"\n";
      OX<<spc<<spc<<"emin = "<<eMin<<"\n";
      OX<<spc<<spc<<"emax = "<<eMax<<"\n";
    }

  return;
}

}   // NAMESPACE phitsSystem
