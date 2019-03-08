/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcess/cellValueSet.cxx
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
#include "cellValueSet.h"

namespace flukaSystem
{
  
template<size_t N>
cellValueSet<N>::cellValueSet(const std::string& KN,
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
cellValueSet<N>::cellValueSet(const std::string& KN,
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
cellValueSet<N>::cellValueSet(const std::string& KN,
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
cellValueSet<N>::cellValueSet(const cellValueSet<N>& A) : 
  keyName(A.keyName),outName(A.outName),tag(A.tag),
  scaleVec(A.scaleVec),dataMap(A.dataMap),
  strRegister(A.strRegister),intRegister(A.intRegister)
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
      dataMap=A.dataMap;
      strRegister=A.strRegister;
      intRegister=A.intRegister;
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
  strRegister.erase(strRegister.begin(),strRegister.end());
  intRegister.erase(intRegister.begin(),intRegister.end());
  return;
}

template<size_t N>    
int
cellValueSet<N>::makeStrIndex(const std::string& CName)
  /*!
    Add a new strRegister entry and return a unique 
    negative index
    \return index number
   */
{
  ELog::RegMethod RegA("cellValueSet","makeStrIndex");
  
  std::map<std::string,int>::const_iterator mc=
    intRegister.find(CName);
  if (mc!=intRegister.end()) return mc->second;

  const int index=1+static_cast<int>(intRegister.size());
  intRegister.emplace(CName,-index);
  strRegister.emplace(-index,CName);
  return -index;
}

template<size_t N>    
const std::string&
cellValueSet<N>::getStrIndex(const int CN) const
  /*!
    Add a new strRegister entry and return a unique 
    negative index
    \param CN :: Cell number
    \return index name
   */
{
  ELog::RegMethod RegA("cellValueSet","getStrIndex");
  
  std::map<int,std::string>::const_iterator mc=
    strRegister.find(CN);
  if (mc==strRegister.end())
    throw ColErr::InContainerError<int>(CN,"Cell number not in register");

  return mc->second;
}
  
template<size_t N>  
bool
cellValueSet<N>::simpleSplit(std::vector<std::tuple<int,int>>& initCell,
			     std::vector<valTYPE>& outData) const
/*!
    Group the individual values into equal ranges. The 
    outData is a list of ranges with equal value. The
    goal of this method is to minimize the number of fluka cards. e.g
    cell 1 to 10 might all have the same importance values, so they can
    be expressed in FLUKA as a single importance/bias card with a range 
    of cells.

    \param initCell :: initialization range
    \param dataValue :: initialization range
    \return true if output required
   */
{
  ELog::RegMethod RegA("cellValueSet","simpleSplit ");
  typedef std::tuple<int,int> TITEM;
  initCell.clear();
  outData.clear();

  if (dataMap.empty()) return 0;
  const int lastCN=dataMap.rbegin()->first+1;       //ordered map

  int prev(0);
  valTYPE V;
  for(int CN=dataMap.begin()->first;CN<=lastCN;CN++)
    {
      typename dataTYPE::const_iterator mc=dataMap.find(CN);	  
      if (mc==dataMap.end())
	{
	  if (prev)
	    {
	      initCell.push_back(TITEM(prev,CN-1));
	      outData.push_back(V);
	      prev=0;
	    }
	}
      else
	{
	  if (prev)
	    {
	      for(size_t index=0;index<N;index++)
		{
		  const int& VDef=V[index].first;
		  const std::string& VS=V[index].second;
		  const int& ADef=mc->second[index].first;
		  const std::string& AS=mc->second[index].second;

		  double VV,AV;
		  int sumFlag=(VDef<1) ? 0 : StrFunc::convert(VS,VV);
		  sumFlag+= (ADef<1) ? 0 : StrFunc::convert(AS,AV);

		  if (VDef!=ADef ||           // diff flag
		      (VDef &&                // def = def is a pass
		       ((sumFlag!=2 && AS!=VS) ||
			(sumFlag==2 && (std::abs(VV-AV)>Geometry::zeroTol))) ))
		    {
		      initCell.push_back(TITEM(prev,CN-1));
		      outData.push_back(V);
		      prev=CN;
		      V=mc->second;
		      break;
		    }
		}
	    }
	  else // new thing just initializs
	    {
	      prev=CN;
	      V=mc->second;
	    }
	}
    }
  return (initCell.empty()) ? 0 : 1;
}
  
template<size_t N>  
bool
cellValueSet<N>::cellSplit(const std::vector<int>& cellN,
			   std::vector<std::tuple<int,int>>& initCell,
			   std::vector<valTYPE>& outData) const
/*!
    Group the individual values into equal ranges. The 
    outData is a list of ranges with equal value. The
    goal of this method is to minimize the number of fluka cards. e.g
    cell 1 to 10 might all have the same importance values, so they can
    be expressed in FLUKA as a single importance/bias card with a range 
    of cells.

    \param cellN :: Cell values [normally excluding 0]
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
	      prev=0;
	    }
	}
      else
	{
	  if (prev)
	    {
	      for(size_t index=0;index<N;index++)
		{
		  const int& VDef=V[index].first;
		  const std::string& VS=V[index].second;
		  const int& ADef=mc->second[index].first;
		  const std::string& AS=mc->second[index].second;

		  double VV,AV;
		  int sumFlag=(VDef<1) ? 0 : StrFunc::convert(VS,VV);
		  sumFlag+= (ADef<1) ? 0 : StrFunc::convert(AS,AV);

		  if (VDef!=ADef ||           // diff flag
		      (VDef &&                // def = def is a pass
		       ((sumFlag!=2 && AS!=VS) ||
			(sumFlag==2 && (std::abs(VV-AV)>Geometry::zeroTol))) ))
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
  dataMap[cN]=A;
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
  A[0].first=1;
  A[0].second=StrFunc::makeString(V);
  dataMap[cN]=A;
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const int cN,
			   const double V,
			   const double V2)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V :: value for cell
    \param V2 :: value for cell
  */
{
  valTYPE A;
  A[0].first=1;
  A[0].second=StrFunc::makeString(V);
  A[1].first=1;
  A[1].second=StrFunc::makeString(V2);
  dataMap[cN]=A;
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
  A[0].first=1;         // 1: values
  A[0].second=StrFunc::makeString(V);
  A[1].first=1;
  A[1].second=StrFunc::makeString(V2);
  A[2].first=1;
  A[2].second=StrFunc::makeString(V3);
  dataMap[cN]=A;
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
    A[0].first=0;
  else
    {
      double D;
      A[0].first= (StrFunc::convert(V,D)) ? 1 : -1;
      A[0].second=V;
    }
  dataMap[cN]=A;
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
  dataMap[cN]=A;
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const int cN,const std::string& V1,
			   const std::string& V2,
			   const std::string& V3)
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
  dataMap[cN]=A;
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const std::string& cStr)
  /*!
    Set a value in the map
    \param cStr :: Cell number   
  */
{  
  const int cN=makeStrIndex(cStr);
  setValues(cN);
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const std::string& cStr,
			   const std::string& V)
  /*!
    Set a value in the map
    \param cStr :: Cell number   
    \param V :: value for cell
  */
{
  const int cN=makeStrIndex(cStr);
  setValues(cN,V);
  return;
}
  
template<size_t N>
void
cellValueSet<N>::setValues(const std::string& cStr,const std::string& V1,
			   const std::string& V2)
  /*!
    Set a value in the map
    \param cN :: Cell number   
    \param V1 :: value for cell
    \param V2 :: value for cell
  */
{
  const int cN=makeStrIndex(cStr);
  setValues(cN,V1,V2);
  return;
}

template<size_t N>
void
cellValueSet<N>::setValues(const std::string& cStr,
			   const std::string& V1,
			   const std::string& V2,
			   const std::string& V3)
  /*!
    Set a value in the map
    \param cStr :: Cell number   
    \param V1 :: value for cell
    \param V2 :: value for cell
    \param V3 :: value for cell    
  */
{
  const int cN=makeStrIndex(cStr);
  setValues(cN,V1,V2,V3);
  return;
}


template<size_t N>
void
cellValueSet<N>::writeFLUKA(std::ostream& OX,
			    const std::string& ControlStr) const 
/*!
    Process is to write keyName ControlStr units 
    \param OX :: Output stream
    \param ControlStr units [%0/%1/%2] for cell range/Value
  */
{
  ELog::RegMethod RegA("cellValueSet","writeFLUKA[no-cell]");
  
  typedef std::tuple<int,int> TITEM;

  std::ostringstream cx;
  std::vector<TITEM> Bgroup;
  std::vector<valTYPE> Bdata;

  if (simpleSplit(Bgroup,Bdata))
    {
      const std::vector<std::string> Units=StrFunc::StrParts(ControlStr);
      std::vector<std::string> SArray(3+N);

      for(size_t index=0;index<Bgroup.size();index++)
	{
	  const TITEM& tc(Bgroup[index]);
	  const valTYPE& dArray(Bdata[index]);

	  const int AA=std::get<0>(tc);
	  const int AB=std::get<1>(tc);
	  SArray[0]=(AA<0) ? getStrIndex(AA) : std::to_string(AA);
	  SArray[1]=(AB<0) ? getStrIndex(AB) : std::to_string(AB);

	  for(size_t i=0;i<N;i++)
	    {
	      if (dArray[i].first==1)
		{
		  double D;
		  StrFunc::convert(dArray[i].second,D);
		  SArray[2+i]=StrFunc::makeString(D*scaleVec[i]);
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
    }
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

      for(size_t index=0;index<Bgroup.size();index++)
	{
	  const TITEM& tc(Bgroup[index]);
	  const valTYPE& dArray(Bdata[index]);

	  const int AA=std::get<0>(tc);
	  const int AB=std::get<1>(tc);
	  SArray[0]=(AA<0) ? getStrIndex(AA) : std::to_string(AA);
	  SArray[1]=(AB<0) ? getStrIndex(AB) : std::to_string(AB);
	  for(size_t i=0;i<N;i++)
	    {
	      if (dArray[i].first==1)
		{
		  double D;
		  StrFunc::convert(dArray[i].second,D);
		  SArray[2+i]=StrFunc::makeString(D*scaleVec[i]);
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
		  (UC[0]=='%' || UC[0]=='R' || UC[0]=='M'
		   || UC[0]=='P'))
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
      
   
