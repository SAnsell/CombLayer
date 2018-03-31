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
#include "writeSupport.h"
#include "MapRange.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "particleConv.h"
#include "cellValueSet.h"

namespace flukaSystem
{
  
template<size_t N>
cellValueSet<N>::cellValueSet(const std::string& KN,const std::string& ON) :
  keyName(KN),outName(ON),whatValue(0.0)
  /*!
    Constructor
    \param KN :: Indentifier
    \param ON :: Output id for FLUKA
  */
{
  scaleVec.fill(1.0);
}

template<size_t N>
cellValueSet<N>::cellValueSet(const std::string& KN,
			      const std::string& ON,
			      const std::string& TT) :
  keyName(KN),outName(ON),tag(TT),whatValue(0.0)
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
cellValueSet<N>::cellValueSet(const std::string& KN,
			      const std::string& ON,
			      const std::string& TT,
			      const double WValue) :
  keyName(KN),outName(ON),tag(TT),whatValue(WValue)
  /*!
    Constructor
    \param KN :: Indentifier
    \param ON :: Output id for FLUKA
    \param WValue :: What value
  */
{
  scaleVec.fill(1.0);
}

template<size_t N>
cellValueSet<N>::cellValueSet(const std::string& KN,
			      const std::string& ON,
			      const std::string& TT,
			      const double WValue,
			      const std::array<double,N>& scaleV) :
  keyName(KN),outName(ON),tag(TT),whatValue(WValue),
  scaleVec(scaleV)
  /*!
    Constructor
    \param KN :: Indentifier
    \param ON :: Output id for FLUKA
    \param WValue :: What value
  */
{}


template<size_t N>
cellValueSet<N>::cellValueSet(const cellValueSet& A) : 
  keyName(A.keyName),outName(A.outName),tag(A.tag),
  whatValue(A.whatValue),scaleVec(A.scaleVec)
  /*!
    Copy constructor
    \param A :: cellValueSet to copy
  */
{}

template<size_t N>  
cellValueSet<N>&
cellValueSet<N>::operator=(const cellValueSet<N>& A)
  /*!
    Assignment operator
    \param A :: cellValueSet to copy
    \return *this
  */
{
  if (this!=&A)
    {
      whatValue=A.whatValue;
      dataMap=A.dataMap;
      scaleVec=A.scaleVec;
    }
  return *this;
}

template<size_t N>
cellValueSet<N>::~cellValueSet()
  /*!
    Destructor
  */
{}

template<size_t N>  
void
cellValueSet<N>::clearAll()
  /*!
    The big reset
  */
{
  dataMap.erase(dataMap.begin(),dataMap.end());  
  return;
}

template<size_t N>  
bool
cellValueSet<N>::cellSplit(const std::vector<int>& cellN,
			   std::vector<std::tuple<int,int>>& initCell,
			   std::vector<valTYPE>& outData) const
/*!
    Process ranges to find for value
    \param cellN :: Cell vlaues
    \param initCell :: initialization range
    \param dataValue :: initialization range
    \return true if output required
   */
{
  typedef std::tuple<int,int> TITEM;
  initCell.clear();
  outData.clear();
 
  if (dataMap.empty() || cellN.empty()) return 0;
  
  size_t prev(0);
  valTYPE V;
  for(size_t i=0;i<cellN.size();i++)
    {
      const int CN=cellN[i];
      typename dataTYPE::const_iterator mc=dataMap.find(CN);
      if (mc==dataMap.end())
	{
	  if (prev)
	    {
	      initCell.push_back(TITEM(cellN[prev-1],cellN[i-1]));
	      outData.push_back(V);
	      V=mc->second;
	      prev=0;
	    }
	}
      else
	{
	  if (prev)
	    {
	      for(size_t index=0;index<N;index++)
		{
		  const bool& VDef=V[index].first;
		  const double& VV=V[index].second;
		  const bool& ADef=mc->second[index].first;
		  const double& AV=mc->second[index].second;

		  // only if not def
		  if (VDef!=ADef || 
		      (VDef==0 && (std::abs(VV-AV)>Geometry::zeroTol))) 
		    {
		      initCell.push_back(TITEM(cellN[prev-1],cellN[i-1]));
		      outData.push_back(V);
		      prev=i+1;
		      V=mc->second;
		      break;
		    }
		}
	    }
	  else if (!prev)
	    {
	      prev=i+1;
	      V=mc->second;
	    }
	}
    }
  
  if (prev)
    {
      initCell.push_back(TITEM(cellN[prev-1],cellN.back()));
      outData.push_back(V);
    }
  
  return (initCell.empty()) ? 0 : 1;
}



template<size_t N>
void
cellValueSet<N>::setValues(const int cN)
  /*!
    Set a value in the map
    \param cN :: Cell number   
  */
{
  valTYPE A;
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const int cN,const double V)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V :: value for cell
  */
{
  valTYPE A;
  A[0].first=0;
  A[0].second=V;
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const int cN,const double V,
			   const double V2)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V :: value for cell
    \param V2 :: value for cell
  */
{
  valTYPE A;
  A[0].first=0;
  A[0].second=V;
  A[1].first=0;
  A[1].second=V2;
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const int cN,const double V,
			   const double V2,const double V3)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V :: value for cell
    \param V2 :: value for cell
    \param V3 :: value for cell
  */
{
  valTYPE A;
  A[0].first=0;
  A[0].second=V;
  A[1].first=0;
  A[1].second=V2;
  A[2].first=0;
  A[2].second=V3;
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const int cN,const std::string& V)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V :: value for cell
  */
{
  valTYPE A;
  if (V=="def" || V=="Def")
    A[0].first=1;
  else
    {
      double D;
      if (!StrFunc::convert(V,D))
	throw ColErr::InvalidLine(V,"No convertion to number");
      A[0].first=0;
      A[0].second=D;
    }
  dataMap.emplace(cN,A);
  return;
}
  
template<size_t N>
void
cellValueSet<N>::setValues(const int cN,const std::string& V1,
			   const std::string& V2)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V1 :: value for cell
    \param V2 :: value for cell
  */
{
  valTYPE A;
  const std::vector<const std::string*> VStr({&V1,&V2});
  for(size_t i=0;i<VStr.size();i++)
    {
      if (*VStr[i]=="def" || *VStr[i]=="Def")
	A[i].first=1;
      else
	{
	  double D;
	  if (!StrFunc::convert(*VStr[i],D))
	    throw ColErr::InvalidLine(*VStr[i],"No convertion to number");
	  A[i].first=0;
	  A[i].second=D;
	}
    }
  dataMap.emplace(cN,A);
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const int cN,const std::string& V1,
			   const std::string& V2,const std::string& V3)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V1 :: value for cell
    \param V2 :: value for cell
    \param V3 :: value for cell    
  */
{
  valTYPE A;
  const std::vector<const std::string*> VStr({&V1,&V2,&V3});
  for(size_t i=0;i<VStr.size();i++)
    {
      if (*VStr[i]=="def" || *VStr[i]=="Def")
	A[i].first=1;
      else
	{
	  double D;
	  if (!StrFunc::convert(*VStr[i],D))
	    throw ColErr::InvalidLine(*VStr[i],"No convertion to number");
	  A[i].first=0;
	  A[i].second=D;
	}
    }
  dataMap.emplace(cN,A);
  return;
}



template<size_t N>
void
cellValueSet<N>::writeFLUKA(std::ostream& OX,
			    const std::vector<int>& cellN,
			    const std::string& ControlStr) const 
/*!
    Process is to write keyName ControlStr units 
    \param OX :: Output stream
    \param cellN :: vector of cells
    \param ControlStr units [%0/%1/%2] for cell range/Value
  */
{
  ELog::RegMethod RegA("cellValueSet","writeFLUKA");
  
  typedef std::tuple<int,int> TITEM;

  std::ostringstream cx;
  std::vector<TITEM> Bgroup;
  std::vector<valTYPE> Bdata;

  if (cellSplit(cellN,Bgroup,Bdata))
    {
      const std::vector<std::string> Units=StrFunc::StrParts(ControlStr);
      std::vector<std::string> SArray(3+N);
      SArray[0]=std::to_string(whatValue);

      for(size_t index=0;index<Bgroup.size();index++)
	{
	  const TITEM& tc(Bgroup[index]);
	  const valTYPE& dArray(Bdata[index]);

	  SArray[1]=std::to_string(std::get<0>(tc));
	  SArray[2]=std::to_string(std::get<1>(tc));
	  for(size_t i=0;i<N;i++)
	    {
	      if (!dArray[i].first)
		SArray[3+i]=std::to_string(dArray[i].second*scaleVec[i]);
	      else
		SArray[3+i]="-";
	    }
	  cx.str("");
	  cx<<outName<<" ";

	  for(const std::string& UC : Units)
	    {
	      if (UC.size()==2 &&
		  (UC[0]=='%' || UC[0]=='R' || UC[0]=='M'))
		{
		  const size_t SA=(static_cast<size_t>(UC[1]-'0') % (N+3));
		  if (UC[0]=='%')
		    cx<<SArray[SA]<<" ";
		  else if (UC[0]=='M' || UC[0]=='R')
		    cx<<UC[0]<<SArray[SA]<<" ";
		}
	      else
		cx<<UC<<" ";
	    }
	  cx<<tag;
	  StrFunc::writeFLUKA(cx.str(),OX);
	}
    }
  return;
}

///\cond TEMPLATE
template class cellValueSet<0>;
template class cellValueSet<1>;
template class cellValueSet<2>;
template class cellValueSet<3>;
///\endcond TEMPLATE
  
} // NAMESPACE flukaSystem
      
   
