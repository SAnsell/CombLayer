/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   input/IItemBase.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MapSupport.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Binary.h"
#include "IItemBase.h"


namespace mainSystem
{

std::ostream&
operator<<(std::ostream& OX,const IItemBase& A)
  /*!
    Output stream writer
    \param OX :: Output stream
    \param A :: ItemBase Object to write
    \return Stream Obj
  */
{
  A.write(OX);
  return OX;
}

IItemBase::IItemBase(const std::string& K) : 
  Key(K)
  /*!
    Constructor only with  descriptor
    \param K :: Key Name
  */
{}

IItemBase::IItemBase(const std::string& K,const std::string& L) :
  Key(K),Long(L)
  /*!
    Full Constructor 
    \param K :: Key Name
    \param L :: Long name
  */
{}

IItemBase::IItemBase(const IItemBase& A) : 
  Key(A.Key),Long(A.Long),Desc(A.Desc)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

IItemBase&
IItemBase::operator=(const IItemBase& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this != &A)
    {
      Key=A.Key;
      Long=A.Long;
      Desc=A.Desc;
    }
  return *this;
}

IItemFlag::IItemFlag(const std::string& K) : 
  IItemBase(K),active(0)
  /*!
    Constructor
    \param K :: Key Name
  */
{}

IItemFlag::IItemFlag(const std::string& K,const std::string& L) :
  IItemBase(K,L),active(0)
  /*!
    Full Constructor
    \param K :: Key Name
    \param L :: Long name
  */
{}

IItemFlag::IItemFlag(const IItemFlag& A) :
  IItemBase(A),active(A.active)
  /*!
    Copy Constructor
    \param A :: Item to copy
  */
{}

IItemFlag&
IItemFlag::operator=(const IItemFlag& A)
  /*!
    Assignment operator
    \param A :: Item to copy
    \return *this
  */
{
  if (this != &A)
    {
      IItemBase::operator=(A);
      active=A.active;
    }
  return *this;
}

IItemBase*
IItemFlag::clone() const 
  /*!
    Virtual copy constructor
    \return new (this)
  */
{ 
  return new IItemFlag(*this);
}

void
IItemFlag::addNewSection() 
  /*!
    Found key adding new section --
    just sets active 
  */
{
  active=1;
  return;
}

size_t
IItemFlag::convert(const size_t,const size_t,
		   const std::vector<std::string>&)
  /*!
    Convert the Unit into the appropiate item.
    \return Always fails as flags can't have components
  */
{
  return 0;
}

// --------------------------------------------------------------

template<typename T>
IItemObj<T>::IItemObj(const size_t dCnt,const std::string& K) : 
  IItemBase(K),N((dCnt<1) ? 1 : dCnt),
  NReq(N),active(0),ObjPtr(new T[N])
  /*!
    Constructor
    \param dCnt :: number of data items;
    \param K :: Short Key
  */
{}

template<typename T> 
IItemObj<T>::IItemObj(const size_t dCnt,const std::string& K,
		      const std::string& L) : 
  IItemBase(K,L),N((dCnt<1) ? 1 : dCnt),
  NReq(N),active(0),ObjPtr(new T[N])
  /*!
    Constructor
    \param dCnt :: number of data items;
    \param K :: Short Key
    \param L :: Long Key
  */
{}

template<typename T>
IItemObj<T>::IItemObj(const size_t dCnt,const size_t reqCnt,
		      const std::string& K) : 
  IItemBase(K),N((dCnt<1) ? 1 : dCnt),
  NReq((reqCnt>N) ? N : reqCnt),
  active(0),ObjPtr(new T[N])
  /*!
    Constructor
    \param dCnt :: number of data items;
    \param reqCnt :: Required number of data items
    \param K :: Short Key
  */
{}

template<typename T>
IItemObj<T>::IItemObj(const size_t dCnt,const size_t reqCnt,
		      const std::string& K,const std::string& L) :
  IItemBase(K,L),N((dCnt<1) ? 1 : dCnt),
  NReq((reqCnt>N) ? N : reqCnt),
  active(0),ObjPtr(new T[N])
  /*!
    Constructor
    \param dCnt :: number of data items;
    \param reqCnt :: Required number of data items
    \param K :: Key Name
    \param L :: Long Key
  */
{}

template<typename T>
IItemObj<T>::IItemObj(const IItemObj<T>& A) : 
  IItemBase(A),N(A.N),NReq(A.NReq),
  active(A.active),ObjPtr(new T[N])
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{
  for(size_t i=0;i<N;i++)
    ObjPtr[i] = A.ObjPtr[i];
}

template<typename T>
IItemObj<T>& 
IItemObj<T>::operator=(const IItemObj<T>& A)
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      IItemBase::operator=(A);
      active=A.active;
      if (N!=A.N)
	delete [] ObjPtr;      
      ObjPtr=new T[N];
      for(size_t i=0;i<N;i++)
	ObjPtr[i]= A.ObjPtr[i];
    }
  return *this;
}
  
template<typename T>
IItemBase*
IItemObj<T>::clone() const 
  /*!
    Virtual copy constructor
    \return new (this)
  */
{ 
  return new IItemObj<T>(*this);
}

template<typename T>
IItemObj<T>::~IItemObj()
  /*!
    Destructor
  */
{ 
  delete [] ObjPtr;
}

template<typename T>
bool
IItemObj<T>::flag() const
  /*!
    Is everything set/default value
    Acitve 
    \return if fully set [with def]
  */
{
  ELog::RegMethod RegA("IItemObj<T>","flag");
  const size_t reqActive((2UL << NReq)-1);
  return ((active & reqActive)==reqActive) ? 1 : 0;
}

template<typename T>
const T&
IItemObj<T>::getObj(const size_t I) const
  /*!
    Get Object
    \param I :: Index
    \return Object
  */
{ 
  // No need to check since called from inputParam
  return ObjPtr[I]; 
}

template<typename T>
void
IItemObj<T>::setObj(const T& A)
  /*!
    Set Object
    \param A :: Object
  */
{ 
  setObj(0,A);
  return;
}
  
template<typename T>
void
IItemObj<T>::setObj(const size_t I,const T& A) 
  /*!
    Set Object
    \param I :: Index
    \param A :: Object
  */
{ 
  // No need to check since called from inputParam
  ObjPtr[I]=A;
  active |= (2UL << I);
  return;
}
  
template<typename T>
void
IItemObj<T>::setDefObj(const size_t I,const T& A) 
  /*!
    Set Object
    \param I :: Index
    \param A :: Object
  */
{ 
  // No need to check since called from inputParam
  ObjPtr[I]=A;
  return;
}

template<typename T>
size_t
IItemObj<T>::dataCnt() const
  /*!
    Checks to see how many flags are set
    \return count of flags
  */
{
  size_t cnt;
  size_t flag(2);
  for(cnt=0;cnt<NReq && (flag & active);cnt++)
    flag <<= 1;
  return (cnt==NReq) ? N : cnt;
}

template<typename T>
size_t
IItemObj<T>::getNCompData(const size_t Index) const
  /*!
    Checks to see how many flags are set
    \param Index :: Index of current group
    \return count of flags
  */
{
  if (!active || Index!=0) 
    return 0;
  return dataCnt();
}

template<typename T>
void
IItemObj<T>::addNewSection()
  /*!
    Processes a new flag
   */
{
  active |= 1;               // set -key flag
  return;
}

template<>
size_t
IItemObj<Geometry::Vec3D>::convert(const size_t Index,
				   const size_t UIndex,
				   const std::vector<std::string>& Units)
  /*!
    Convert the Unit into the appropiate item.
    \param Index :: Object index
    \param UIndex :: Position in vector
    \param Units :: Vector of input
    \return 3 on success and 0 on failure
  */
{
  ELog::RegMethod RegA("IItemObj<T>","convert");

  std::string ConcUnit(Units[UIndex]);
  for(size_t i=1;i<3 &&	(i+UIndex < Units.size());i++)
    ConcUnit+=" "+Units[UIndex+i];
      
  if (StrFunc::convert(ConcUnit,ObjPtr[Index]))
    {
      active |= 2UL << Index;
      return 3;
    }
  return 0;
}

template<>
size_t
IItemObj<std::string>::convert(const size_t Index,const size_t UIndex,
			       const std::vector<std::string>& Units)
  /*!
    Convert the Unit into the appropiate item.
    Note, if Index < 0 then it is part of the flag checking
    sequence.
    \param Index :: Object index
    \param UIndex :: Units position
    \param Units :: Vector of units
    \return 1 on success and 0 on failure
  */
{
  ELog::RegMethod RegA("IItemObj<T>","convert");

  // If empty / -value return 0
  if (Units[UIndex].empty() ||
      (Units[UIndex].size()>1 && 
       Units[UIndex][0]=='-'&&
       !isdigit(Units[UIndex][1])))
      return 0;
  

  if (StrFunc::convert(Units[UIndex],ObjPtr[Index]))
    {
      active |= 2UL << Index;
      return 1;
    }
  return 0;
}

template<typename T>
size_t
IItemObj<T>::convert(const size_t Index,const size_t UIndex,
		     const std::vector<std::string>& Units)
  /*!
    Convert the Unit into the appropiate item.
    Note, if Index < 0 then it is part of teh flag checking
    sequence.
    \param Index :: Object index
    \param UIndex :: Units position
    \param Units :: Vector of units
    \return 1 on success and 0 on failure
  */
{
  ELog::RegMethod RegA("IItemObj<T>","convert");

  if (StrFunc::convert(Units[UIndex],ObjPtr[Index]))
    {
      active |= (2UL << Index);
      return 1;
    }
  return 0;
}

template<typename T>
void
IItemObj<T>::write(std::ostream& OX) const
  /*!
    Complex functiion to convert the system into a string
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("IItemObj<T>","write");

  size_t index(2);
  for(size_t i=0;i<N;i++,index <<= 1)
    {
      if (active & index)
	OX<<ObjPtr[i]<<" ";
      else if (i>=NReq)
	OX<<"("<<ObjPtr[i]<<") ";
      else
	OX<<" -- ";
    }
  return;
}

// --------------------------------------------------------------
 
template<typename T>
IItemMulti<T>::IItemMulti(const std::string& K,const size_t NG,
			  const size_t NR) : 
  IItemBase(K),nRead(NG),nReq(NR),NGrp(0),active(0)
  /*!
    Constructor
    \param K :: Short Key
    \param NG :: Number in each group
    \param NR :: Number required in each group
  */
{}

template<typename T>
IItemMulti<T>::IItemMulti(const std::string& K,const std::string& L,
			  const size_t NG,const size_t NR) : 
  IItemBase(K,L),nRead(NG),nReq(NR),NGrp(0),active(0)
  /*!
    Constructor
    \param K :: Short Key
    \param L :: Long Key
    \param NG :: Number in each group
    \param NR :: Number required in each group
  */
{}

template<typename T>
IItemMulti<T>::IItemMulti(const IItemMulti<T>& A) : 
  IItemBase(A),nRead(A.nRead),nReq(A.nReq),NGrp(A.NGrp),
  active(A.active)  
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{
  for(size_t i=0;i<ObjVec.size();i++)
    {
      ObjVec.push_back(ITYPE(nRead));
      for(size_t j=0;j<nRead;j++)
	ObjVec.back()[j]=(A.ObjVec[i][j] ? new T(*A.ObjVec[i][j]) : 0);
    }
}

template<typename T>
IItemMulti<T>& 
IItemMulti<T>::operator=(const IItemMulti<T>& A)
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      IItemBase::operator=(A);
      active=A.active;
      NGrp=A.NGrp;
      deleteVec();
      for(size_t i=0;i<ObjVec.size();i++)
	{
	  ObjVec.push_back(ITYPE(nRead));
	  for(size_t j=0;j<nRead;j++)
	    ObjVec.back()[j]=(A.ObjVec[i][j] ? new T(*A.ObjVec[i][j]) : 0);
	}
    }
  return *this;
}
 
template<typename T>
IItemBase*
IItemMulti<T>::clone() const 
  /*!
    Virtual copy constructor
    \return new (this)
  */
{ 
  return new IItemMulti<T>(*this);
}

template<typename T>
IItemMulti<T>::~IItemMulti()
  /*!
    Destructor
  */
{ 
  deleteVec();
}

template<typename T>
void
IItemMulti<T>::deleteVec()
  /*!
    Delete all the items
  */
{
  typename VTYPE::iterator vc;
  for(vc=ObjVec.begin();vc!=ObjVec.end();vc++)
    {
      typename ITYPE::iterator ic;
      for(ic=vc->begin();ic!=vc->end();ic++)
	delete *ic;
    }
  ObjVec.clear();
  NGrp=0;
  active=0;
  return;
}

template<typename T>
bool
IItemMulti<T>::flag() const
  /*!
    Is everything set/default value
    \return if fully set [with def]
  */
{
  return active;
}

template<typename T>
const T&
IItemMulti<T>::getObj(const size_t Index) const
  /*!
    Get Object
    \param Index :: Full index value
    \return Object
  */
{ 
  return getObj(Index/nRead,Index%nRead);
}

template<typename T>
const T&
IItemMulti<T>::getObj(const size_t IG,const size_t II) const
  /*!
    Get Object
    \param IG :: Index of group
    \param II :: Index of item
    \return Object
  */
{ 
  ELog::RegMethod RegA("IItemMulti","getObj");
  // No need to check since called from inputParam
  if (IG>=NGrp || II>=nRead)
    throw ColErr::IndexError<size_t>(II,IG,"II/IG");

  if (!ObjVec[IG][II])
    throw ColErr::EmptyValue<T*>("ObjVec empty");

  return *ObjVec[IG][II];
}
  
template<typename T>
void
IItemMulti<T>::setObj(const T& A) 
  /*!
    Set Object
    \param A :: Object
  */
{ 
  ELog::RegMethod RegA("IItemMulti","setObj(IMulti)");
  
  if (ObjVec.empty() || ObjVec.back().back())
    {
      ObjVec.push_back(ITYPE(nRead));
      std::fill(ObjVec.back().begin(),ObjVec.back().end(),
		static_cast<T*>(0));
      setObj(NGrp,0,A);
      NGrp++;
      return;
    }

  // This must work
  for(size_t i=0;i<nRead;i++)
    {
      if (!ObjVec.back()[i])
	{
	  setObj(NGrp-1,i,A);
	  return;
	}
    }

  return;
}

template<typename T>
void
IItemMulti<T>::setObj(const size_t IGrp,const size_t IItem,const T& A) 
  /*!
    Set Object
    \param IGrp :: Index of group
    \param IItem :: Index of item
    \param A :: Object
  */
{ 
  ELog::RegMethod RegA("IItemMulti","setObj(int,int,A)");

  // Error conditions:

  if (IGrp>=ObjVec.size())
    throw ColErr::IndexError<size_t>(IGrp,ObjVec.size(),
				     "IGrp beyond ObjVec.size()");
  if (IItem>=nRead)
    throw ColErr::IndexError<size_t>(IItem,nRead,"IItem at nRead");

  delete ObjVec[IGrp][IItem];
  ObjVec[IGrp][IItem] = new T(A);

  return;
}
  
template<typename T>
size_t
IItemMulti<T>::dataCnt() const
  /*!
    Checks to see how many flags are set
    \return count of flags
  */
{
  return (active) ? NGrp*nRead : 0;
}

template<typename T>
size_t
IItemMulti<T>::getNCompData(const size_t Index) const
  /*!
    Checks to see how many flags are set
    \param Index :: Index of current group
    \return count of flags
  */
{
  if ( !active || Index >= NGrp)
    return 0;
  const ITYPE& OV=ObjVec[Index];
  size_t i;
  for(i=0;i<OV.size() && OV[i]; i++) ;

  return i;
}

template<typename T>
void
IItemMulti<T>::addNewSection()
  /*!
    Adds a new component
  */
{
  active |=1;
  ObjVec.push_back(ITYPE(nRead));
  std::fill(ObjVec.back().begin(),ObjVec.back().end(),
	    static_cast<T*>(0));
  NGrp++;
  return;
}

template<typename T>
size_t
IItemMulti<T>::convert(const size_t ,
		       const size_t UIndex,
		       const std::vector<std::string>& Units)
  /*!
    Convert the Unit into the appropiate item.
    \param :: Object index
    \param UIndex :: position in input
    \param Units :: Vector of inputs
    \retval 1 on success and 0 on failure
  */
{
  ELog::RegMethod RegA("IItemMulti<T>","convert");

  T Obj;
  if (!Units[UIndex].empty() &&
      StrFunc::convert(Units[UIndex],Obj))
    {
      setObj(Obj);
      return 1;
    }
  return 0;
}

template<>
size_t
IItemMulti<std::string>::convert(const size_t,
				 const size_t UIndex,
				 const std::vector<std::string>& Units)
  /*!
    Convert the Unit into the appropiate item.
    \param  :: Object index
    \param UIndex :: position in input
    \param Units :: Vector of inputs
    \retval 1 on success and 0 on failure
  */
{
  ELog::RegMethod RegA("IItemMulti<std::string>","convert");

  // If empty / -value return 0
  if (Units[UIndex].empty() ||
      (Units[UIndex].size()>1 && 
       Units[UIndex][0]=='-'&&
       !isdigit(Units[UIndex][1])))
      return 0;

  const std::string Obj=StrFunc::fullBlock(Units[UIndex]);
  if (!Obj.empty())
    {
      setObj(Obj);
      return 1;
    }
  return 0;
}

template<>
size_t
IItemMulti<Geometry::Vec3D>::convert(const size_t,
				     const size_t UIndex,
				     const std::vector<std::string>& Units)
  /*!
    Convert the Unit into the appropiate item.
    Note, if Index < 0 then it is part of the flag checking
    sequence.
    \param :: Object index
    \param UIndex :: Position in vector
    \param Units :: Vector of input items
    \return 1 on success and 0 on failure
  */
{
  ELog::RegMethod RegA("IItemMulti<Vec3D>","convert");

  std::string ConcUnit(Units[UIndex]);
  for(size_t i=1;i<3 && ((i+UIndex) < Units.size());i++)
    ConcUnit+=" "+Units[UIndex+i];

  Geometry::Vec3D Obj;
  if (StrFunc::convert(ConcUnit,Obj))
    {
      setObj(Obj);
      return 1;
    }
  return 0;
}

template<typename T>
void
IItemMulti<T>::write(std::ostream& OX) const
  /*!
    Complex functiion to convert the system into a stream
    \param OX :: Out put stream
  */
{
  ELog::RegMethod RegA("IItemMulti<T>","write");
  for(size_t i=0;i<NGrp;i++)
    {
      for(size_t j=0;j<nRead;j++)
	if (ObjVec[i][j])
	  OX<<*ObjVec[i][j]<<" ";
	else if (j<nReq)
	  OX<<"NULL ";
      OX<<":: ";
    }
  return;
}

///\cond TEMPLATE

template class IItemObj<int>;
template class IItemObj<long int>;
template class IItemObj<unsigned int>;
template class IItemObj<size_t>;
template class IItemObj<double>;
template class IItemObj<std::string>;
template class IItemObj<Geometry::Vec3D>;

template class IItemMulti<long int>;
template class IItemMulti<int>;
template class IItemMulti<unsigned int>;
template class IItemMulti<size_t>;
template class IItemMulti<double>;
template class IItemMulti<std::string>;
template class IItemMulti<Geometry::Vec3D>;

///\endcond TEMPLATE
 
}  // NAMESPACE mainSystem


