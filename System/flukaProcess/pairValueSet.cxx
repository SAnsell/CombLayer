/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/cellValueSet.cxx
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory> 
#include <array>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "writeSupport.h"
#include "MapRange.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "particleConv.h"
#include "pairValueSet.h"

namespace flukaSystem
{
  
template<size_t N>
pairValueSet<N>::pairValueSet(const std::string& KN,
			      const std::string& ON) :
  keyName(KN),outName(ON)
  /*!
    Constructor
    \param KN :: Indentifier
    \param ON :: Output id for FLUKA
  */
{
  scaleVec.fill(1.0);
}

template<size_t N>
pairValueSet<N>::pairValueSet(const std::string& KN,
			      const std::string& ON,
			      const std::string& TT) :
  keyName(KN),outName(ON),tag(TT)
  /*!
    Constructor
    \param KN :: Indentifier
    \param ON :: Output id for FLUKA
    \param TT :: Tag name
  */
{
  scaleVec.fill(1.0);
}

template<size_t N>
pairValueSet<N>::pairValueSet(const std::string& KN,
			   const std::string& ON,
			   const std::string& TT,
			   const std::array<double,N>& scaleV) :
  keyName(KN),outName(ON),tag(TT),scaleVec(scaleV)
  /*!
    Constructor
    \param KN :: Indentifier
    \param ON :: Output id for FLUKA
    \param TT :: Tag name
    \param scaleV :: Scale for double values
  */
{}


template<size_t N>
pairValueSet<N>::pairValueSet(const pairValueSet<N>& A) : 
  keyName(A.keyName),outName(A.outName),tag(A.tag),
  scaleVec(A.scaleVec),dataMap(A.dataMap)
  /*!
    Copy constructor
    \param A :: pairValueSet to copy
  */
{}

template<size_t N>  
pairValueSet<N>&
pairValueSet<N>::operator=(const pairValueSet<N>& A)
  /*!
    Assignment operator
    \param A :: pairValueSet to copy
    \return *this
  */
{
  if (this!=&A)
    {
      dataMap=A.dataMap;
      scaleVec=A.scaleVec;
    }
  return *this;
}

template<size_t N>
pairValueSet<N>::~pairValueSet()
  /*!
    Destructor
  */
{}

template<size_t N>  
void
pairValueSet<N>::clearAll()
  /*!
    The big reset
  */
{
  dataMap.clear();
  return;
}
 
template<size_t N>
void
pairValueSet<N>::setValues(const std::string& S1 ,const std::string& S2,
			   const std::string& S3,const double V1,
			   const double V2,const double V3)
  /*!
    Basic 3 set values
    \param S1 :: String 
    \param S2 :: String 
    \param S3 :: String 
    \param V1 :: Value  
    \param V2 :: Value 
    \param V3 :: Value 
   */
{
  const std::string* SItem[3]={&S1,&S2,&S3};
  valTYPE A;
  for(size_t i=0;i<3 && i<N;i++)
    {
      const std::string& SV=*SItem[i];
      
      if (SV == "def" || SV =="Def")
	A[i].first= 0;
      else
	{
	  A[i].first=-1;
	  A[i].second= SV;
	}
    }
  
  const double DItem[3]={V1,V2,V3};
  for(size_t i=0;i<3 && i+3<N;i++)
    {
      A[i+3].first=1;
      A[i+3].second= StrFunc::makeString(DItem[i]);
    }

  // check if equal
  dataMap.push_back(A);
  return;
}

template<size_t N>
void
pairValueSet<N>::setValues(const std::string& S1 ,const std::string& S2,
			   const std::string& S3,const std::string& V1,
			   const std::string& V2,const std::string& V3)
  /*!
    Basic 3 set values
    \param S1 :: String 
    \param S2 :: String 
    \param S3 :: String 
    \param V1 :: Value  
    \param V2 :: Value 
    \param V3 :: Value 
   */
{
  const std::string* SItem[6]={&S1,&S2,&S3,&V1,&V2,&V3};


  valTYPE A;
  for(size_t i=0;i<6 && i<N;i++)
    {

      const std::string& SV=*SItem[i];
      if (SV == "def" || SV =="Def")
	A[i].first= 0;
      else
	{
	  A[i].first=-1;
	  A[i].second= SV;
	}
    }
  // check if equal
  dataMap.push_back(A);
  return;
}
      
template<size_t N>
std::string
pairValueSet<N>::pairUnit(const double& scaleValue,
			  const std::pair<int,std::string>& PVal) 

  /*!
    Process a single pair unit
    \param scaleValue :: Scale value for double values
    \param PVal :: pair of type[-1:string/0:def/1:double] / string 
    \return string of value
  */
{
  ELog::RegMethod RegA("pairValueSet","pairUnit");
  const int index=PVal.first;
  double DV;
  
  if (index==0)
    return "-";
  
  if (index==1 && StrFunc::convert(PVal.second,DV))
    return StrFunc::makeString(DV*scaleValue);

  return PVal.second;
}

  
template<size_t N>
void
pairValueSet<N>::writeFLUKA(std::ostream& OX,
			    const std::string& ControlStr) const 
/*!
    Process is to write keyName ControlStr units 
    \param OX :: Output stream
    \param ControlStr units [%0/%1/%2] for cell range/Value
  */
{
  ELog::RegMethod RegA("pairValueSet","writeFLUKA[no-cell]");
  const std::vector<std::string> Units=StrFunc::StrParts(ControlStr);
  
  std::ostringstream cx;
  std::array<std::string,N> SArray;
  for(const valTYPE& V : dataMap)
    {
      cx.str("");
      cx<<outName<<" ";

      for(size_t i=0;i<N;i++)
	SArray[i]=pairUnit(scaleVec[i],V[i]);
      
      
      for(const std::string& UC : Units)
	{
	  if (UC.size()==2 &&
	      (UC[0]=='%' || UC[0]=='R' ||
	       UC[0]=='M' || UC[0]=='P'))
	    {
	      const size_t SA=(static_cast<size_t>(UC[1]-'0') % (N+2));
	      if (UC[0]=='%')
		cx<<SArray[SA]<<" ";
	      else if (UC[0]=='M' || UC[0]=='R')
		cx<<UC[0]<<SArray[SA]<<" ";
	      else if (UC[0]=='P')
		cx<<StrFunc::toUpperString(SArray[SA])<<" ";
	    }
	  else
	    cx<<UC<<" ";
	}
      cx<<tag;
      StrFunc::writeFLUKA(cx.str(),OX);
    }
  return;
}


///\cond TEMPLATE
//template class pairValueSet<0>;
template class pairValueSet<4>;
template class pairValueSet<6>;
//template class pairValueSet<2>;
//template class pairValueSet<3>;
///\endcond TEMPLATE
  
} // NAMESPACE flukaSystem
      
   
