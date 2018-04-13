/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/strValueSet.cxx
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
#include "writeSupport.h"
#include "MapRange.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "flukaGenParticle.h"
#include "strValueSet.h"

namespace flukaSystem
{
  
template<size_t N>
strValueSet<N>::strValueSet(const std::string& KN,
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
strValueSet<N>::strValueSet(const std::string& KN,
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
strValueSet<N>::strValueSet(const std::string& KN,
			    const std::string& ON,
			    const std::string& TT,
			    const std::array<double,N>& scaleV) :
  keyName(KN),outName(ON),tag(TT),scaleVec(scaleV)
  /*!
    Constructor
    \param KN :: Indentifier
    \param ON :: Output id for FLUKA
    \param TT :: Tag name
    \param scaleV :: scales
  */
{}


template<size_t N>
strValueSet<N>::strValueSet(const strValueSet& A) : 
  keyName(A.keyName),outName(A.outName),tag(A.tag),
  scaleVec(A.scaleVec),dataMap(A.dataMap)
  /*!
    Copy constructor
    \param A :: strValueSet to copy
  */
{}

template<size_t N>  
strValueSet<N>&
strValueSet<N>::operator=(const strValueSet<N>& A)
  /*!
    Assignment operator
    \param A :: strValueSet to copy
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
strValueSet<N>::~strValueSet()
  /*!
    Destructor
  */
{}

template<size_t N>  
void
strValueSet<N>::clearAll()
  /*!
    The big reset
  */
{
  dataMap.erase(dataMap.begin(),dataMap.end());  
  return;
}

template<size_t N>
void
strValueSet<N>::setValues(const std::string& cN)
  /*!
    Set a value in the map
    \param cN :: Cell name
  */
{
  valTYPE A;
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
strValueSet<N>::setValues(const std::string& cN,const double V)
  /*!
    Set a value in the map
    \param cN :: Cell name 
    \param V :: value for cell
  */
{
  valTYPE A;
  A[0].first=1;
  A[0].second=std::to_string(V);
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
strValueSet<N>::setValues(const std::string& cN,const double V,
			   const double V2)
  /*!
    Set a value in the map
    \param cN :: Cell name
    \param V :: value for cell
    \param V2 :: value for cell
  */
{
  valTYPE A;
  A[0].first=1;
  A[0].second=std::to_string(V);
  A[1].first=1;
  A[1].second=std::to_string(V2);
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
strValueSet<N>::setValues(const std::string& cN,const double V,
			   const double V2,const double V3)
  /*!
    Set a value in the map
    \param cN :: Cell name   
    \param V :: value for cell
    \param V2 :: value for cell
    \param V3 :: value for cell
  */
{
  valTYPE A;
  A[0].first=1;         // 1: values
  A[0].second=std::to_string(V);
  A[1].first=1;
  A[1].second=std::to_string(V2);
  A[2].first=1;
  A[2].second=std::to_string(V3);
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
strValueSet<N>::setValues(const std::string& cN,const std::string& V)
  /*!
    Set a value in the map
    \param cN :: Cell name
    \param V :: value for cell
  */
{
  valTYPE A;
  if (V=="def" || V=="Def")
    A[0].first=0;
  else
    {
      double D;
      A[0].first= (StrFunc::convert(V,D)) ? 1 : -1;
      A[0].second=V;
    }
  dataMap.emplace(cN,A);
  return;
}
  
template<size_t N>
void
strValueSet<N>::setValues(const std::string& cN,const std::string& V1,
			  const std::string& V2)
  /*!
    Set a value in the map
    \param cN :: Cell name
    \param V1 :: value for cell
    \param V2 :: value for cell
  */
{
  valTYPE A;
  const std::vector<const std::string*> VStr({&V1,&V2});
  for(size_t i=0;i<VStr.size() && i<N;i++)
    {
      if (*VStr[i]=="def" || *VStr[i]=="Def")
	A[i].first=0;
      else
	{
	  double D;
	  A[i].first= (StrFunc::convert(*VStr[i],D)) ? 1 : -1;
	  A[i].second= *VStr[i];
	}
    }
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
strValueSet<N>::setValues(const std::string& cN,const std::string& V1,
			  const std::string& V2,const std::string& V3)
  /*!
    Set a value in the map
    \param cN :: Cell name   
    \param V1 :: value for cell
    \param V2 :: value for cell
    \param V3 :: value for cell    
  */
{
  valTYPE A;
  const std::vector<const std::string*> VStr({&V1,&V2,&V3});
  for(size_t i=0;i<VStr.size() && i<N;i++)
    {
      if (*VStr[i]=="def" || *VStr[i]=="Def")
	A[i].first=0;
      else
	{
	  double D;
	  A[i].first= (StrFunc::convert(*VStr[i],D)) ? 1 : -1;
	  A[i].second= *VStr[i];
	}
    }
  dataMap.emplace(cN,A);
  return;
}



template<size_t N>
void
strValueSet<N>::writeFLUKA(std::ostream& OX,
			   const std::string& ControlStr) const 
/*!
    Process is to write keyName ControlStr units 
    \param OX :: Output stream
    \param ControlStr units [%0/%1/%2] for cell range/Value
  */
{
  ELog::RegMethod RegA("strValueSet","writeFLUKA[no-cell]");
  
  typedef std::tuple<int,int> TITEM;

  std::ostringstream cx;

  const std::vector<std::string> Units=StrFunc::StrParts(ControlStr);
  std::vector<std::string> SArray(3+N);

  for(const typename dataTYPE::value_type& mc : dataMap)
    {
      const valTYPE& dArray(mc.second);
      
      SArray[0]=mc.first;
      SArray[1]=mc.first;
      for(size_t i=0;i<N;i++)
	{
	  if (dArray[i].first==1)
	    {
	      double D;
	      StrFunc::convert(dArray[i].second,D);
	      SArray[2+i]=std::to_string(D*scaleVec[i]);
	    }
	  else if (dArray[i].first == -1)
	    SArray[2+i]=dArray[i].second;
	  else
	    SArray[2+i]="-";
	}
      cx.str("");
      cx<<outName<<" ";
      
      for(const std::string& UC : Units)
	{
	  if (UC.size()==2 &&
	      (UC[0]=='%' || UC[0]=='R' || UC[0]=='M'))
	    {
	      const size_t SA=(static_cast<size_t>(UC[1]-'0') % (N+2));
	      if (UC[0]=='%')
		cx<<SArray[SA]<<" ";
	      else if (UC[0]=='M' || UC[0]=='R')
		cx<<UC[0]<<SArray[SA]<<" ";
	      else if (UC[0]=='P')
		cx<<SArray[SA]<<" ";
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
//template class strValueSet<0>;
template class strValueSet<1>;
//template class strValueSet<2>;
//template class strValueSet<3>;
///\endcond TEMPLATE
  
} // NAMESPACE flukaSystem
      
   
