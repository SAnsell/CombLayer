#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>      
#include <complex>
#include <string>      
#include <list>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <ranges>
#include <functional>
#include <algorithm>
#include <type_traits>

#include "Exception.h"
#include "doubleErr.h"
#include "IndexCounter.h"
#include "dataSlice.h"
#include "multiIndex.h"
#include "multiData.h"

template<typename T>
std::ostream&
operator<<(std::ostream& OX,const multiData<T>& A)
  /*!
    Write out the mirror
    \param OX :: Output stream
    \param A :: dataStore
   */
{
  A.write(OX);
  return OX;
}

template<typename T>
multiData<T>::multiData(std::vector<size_t> I) :
  index(std::move(I)),strides(index.size()+1)  
  /*!
    Constructor with full size
    with array index and data
    Note the extra +1 in stride for zero end
    \param I :: indexes
  */
{
  strides[index.size()]=0;   // always valid
  if (!index.empty())
    {
      size_t prod=1;
      for(size_t i=index.size();i>0;i--)
	{
	  strides[i-1]=prod;
	  prod*=index[i-1];
	}
      flatData.resize(strides[0]*index[0]);
    }
}

template<typename T>
multiData<T>::multiData(std::vector<size_t> I,std::vector<T> D) :
  index(std::move(I)),strides(index.size()+1),
  flatData(std::move(D))
  /*!
    Constructor with full size
    with array index and data
  */
{
  strides[index.size()]=0;   // always valid
  size_t prod=1;
  for(size_t i=index.size();i>0;i--)
    {
      strides[i-1]=prod;
      prod*=index[i-1];
    }

  if (flatData.size()!=strides[0]*index[0])
    throw ColErr::SizeError<size_t>
      (flatData.size(),strides[0]*index[0],"MulitData constructor (ND)");
}

template<typename T>
multiData<T>::multiData()
  /*!
    Constructor with EMPTY
  */
{}

template<typename T>
multiData<T>::multiData(const size_t A) :
  index({A}),strides({1,0}),
  flatData(A)
  /*!
    Constructor with full size (1D)
    \param A :: size of first item
  */
{}

template<typename T>
multiData<T>::multiData(const size_t A,const size_t B) :
  index({A,B}),strides({B,1,0}),
  flatData(A*B)
  /*!
    Constructor with full size (2D)
    \param A :: size of first item
    \param B :: size of first item
  */
{}

template<typename T>
multiData<T>::multiData(const size_t A,const size_t B,
			const size_t C) :
  index({A,B,C}),strides({B*C,C,1,0}),
  flatData(A*B*C)
  /*!
    Constructor with full size
    \param A :: size of first item
    \param B :: size of first item
    \param C :: size of first item
  */
{}

template<typename T>
multiData<T>::multiData(const size_t A,const size_t B,
			const size_t C,const size_t D) :
  index({A,B,C,D}),strides({B*C*D,C*D,D,1,0}),
  flatData(A*B*C*D)
  /*!
    Constructor with full size
    \param A :: size of first item
    \param B :: size of second item
    \param C :: size of third item
    \param D :: size of fourth item
  */
{}

template<typename T>
multiData<T>::multiData(const size_t A,std::vector<T> D) :
  index({A}),strides({1,0}),
  flatData(std::move(D))
  /*!
    Constructor with full size
    \param A :: size of first (only) item
  */
{
  
  if (flatData.size()!=strides[0]*index[0])
    throw ColErr::SizeError<size_t>
      (flatData.size(),strides[0]*index[0],"MulitData constructor (1D)");
}

template<typename T>
multiData<T>::multiData(const size_t A,const size_t B,
			std::vector<T> D) :
  index({A,B}),strides({B,1,0}),
  flatData(std::move(D))
  /*!
    Constructor with full size
    \param A :: size of first item
    \param B :: size of second item
    \param C :: size of third item
  */
{
  if (flatData.size()!=strides[0]*index[0])
    throw ColErr::SizeError<size_t>
	(flatData.size(),strides[0]*index[0],"MulitData constructor (2D)");
}

template<typename T>
multiData<T>::multiData(const size_t A,const size_t B,
			const size_t C,std::vector<T> D) :
  index({A,B,C}),strides({B*C,C,1,0}),
  flatData(std::move(D))
  /*!
    Constructor with full size
    \param A :: size of first item
    \param B :: size of second item
    \param C :: size of third item
  */
{
  if (flatData.size()!=strides[0]*index[0])
    {
      std::cout<<"Index "<<index[0]<<" "<<index[1]<<" "<<index[2]<<
	std::endl;
      std::cout<<"Index "<<strides[0]<<" "<<strides[1]<<" "<<strides[2]<<
	std::endl;
      throw ColErr::SizeError<size_t>
	(flatData.size(),strides[0]*index[0],"MulitData constructor (3D)");
    }
}

template<typename T>
template<typename U>
multiData<T>::multiData(const size_t A,
			const std::vector<U>& D) :
  index({A}),strides({1,0}),
  flatData(A)
  /*!
    Constructor with full size allowing translation from
    type U to type T.
    \param A :: size of first (only) item
  */
{
  if (D.size()<A)
    throw ColErr::SizeError<size_t>
      (A,D.size(),"MulitData type-conv constructor (1D)");

  for(size_t i=0;i<A;i++)
    flatData[i]=static_cast<T>(D[i]);
}

template<typename T>
multiData<T>::multiData(const std::vector<std::vector<T>>& D) 
  /*!
    Constructor with full size
    \param A :: size of first item
    \param B :: size of second item
    \param D :: data
  */
{
  if (!D.empty() && !D.front().empty())
    {
      index={D.size(),D.front().size()};
      strides={index[1],1,0};

      const size_t FData(index[0]*strides[0]);
      flatData.resize(FData);
      size_t I=0;
      for(const std::vector<T>& item : D)
	for(const T& v : item)
	  {
	    flatData[I]=v;
	    I++;
	  }
    }
}

template<typename T>
multiData<T>::multiData
(const std::vector<std::vector<std::vector<T>>>& D) 
  /*!
    Constructor with full size [3D]
    \param D :: data
  */
{  
  if (!D.empty() && !D.front().empty() && !D.front().front().empty())
    {
      index={D.size(),
	     D.front().size(),
	     D.front().front().size()};
      strides={index[1]*index[2],index[2],1,0};

      const size_t FData(index[0]*strides[0]);
      flatData.resize(FData);
      size_t I=0;
      for(const std::vector<std::vector<T>>& itemA : D)
	for(const std::vector<T>& itemB : itemA)
	  for(const T& v : itemB)
	    {
	      flatData[I]=v;
	      I++;
	    }
    }
}

template<typename T>
multiData<T>::multiData(const multiData& A) :
  index(A.index),strides(A.strides),flatData(A.flatData)
  /*!
    Copy constructor
    \param A :: multiData object to copy
  */
{
}

template<typename T>
multiData<T>::multiData(multiData<T>&& A) :
  index(std::move(A.index)),
  strides(std::move(A.strides)),
  flatData(std::move(A.flatData))
  /*!
    Copy constructor
    \param A :: multiData object to copy
  */
{}

template<typename T>
template<typename U>
multiData<T>::multiData(const multiData<U>& A) :
  index(A.shape()),
  strides(A.sVec()),
  flatData(A.getVector().size())
  /*!
    Copy constructor
    \param A :: multiData object to copy
  */
{
  const std::vector<U>& fData=A.getVector();
  std::transform(fData.begin(),fData.end(),flatData.begin(),
		 [](const U& item) { return static_cast<T>(item); });
}

template<typename T>
multiData<T>&
multiData<T>::operator=(const multiData<T>& A)
  /*!
    Assignment operator
    \param A :: multiData object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      strides=A.strides;
      flatData=A.flatData;
    }
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::operator=(multiData<T>&& A)
  /*!
    Assignment operator
    \param A :: multiData object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      index=std::move(A.index);
      strides=std::move(A.strides);
      flatData=std::move(A.flatData);
    }
  return *this;
}

template<typename T>
multiData<T>::~multiData()
  /*!
    Destructor
  */
{}

template<typename T>
void
multiData<T>::throwMatchCheck(const multiData<T>& A,
			      const std::string& callUnit) const
  /*!
    Check if two mult-arrays are equal size [does not check
    range]
    \param A :: multidata to check
    \param callUnit :: name of calling function
    \throw Dimension / MisMatch error if index wrong shape / size
  */
{
  if (index.size()!=A.index.size())
    throw ColErr::MisMatch<size_t>
      (index.size(),A.index.size(),callUnit+" dimenstion");
  
  bool flagSimple(1);
  for(size_t i=0;flagSimple && i<index.size();i++)
    flagSimple=(index[i]==A.index[i]);
  if (!flagSimple)
    throw ColErr::DimensionError<size_t>
      (index,A.index,callUnit+" dimension");
  return;
}

template<typename T>
void
multiData<T>::setStrides(std::vector<size_t> newIndex)
 /*!
   Internal function to set the index/stride without changing
   anything
 */
{
  if (!newIndex.empty())
    {
      index=std::move(newIndex);
      strides.resize(index.size()+1);
      strides[index.size()]=0;   // always valid
      size_t prod=1;
      for(size_t i=index.size();i>0;i--)
	{
	  strides[i-1]=prod;
	  prod*=index[i-1];
	}
    }
  return;
}

template<typename T> 
void
multiData<T>::clear()
  /*!
    Clears all the memory -- not 100% sure that
    it works with zero points
   */
{
  flatData.clear();
  index.clear();
  strides.resize(1);
  strides[0]=0;
  return;
}

template<typename T>
multiData<T>&
multiData<T>::operator+=(const multiData<T>& A)
  /*!
    Addition of other multiData
  */
{
  if (isEmpty())
    return this->operator=(A);

  throwMatchCheck(A,"operator+=");
  
  if constexpr (requires (T a,T b) { a+b; })
    {
      std::transform(flatData.begin(),flatData.end(),
		     A.flatData.begin(),flatData.begin(),
		     [](const T& a,const T& b) ->T
		     { return a+b; }
		     );
    }
  else
    {
      throw ColErr::AbsObjMethod
	("MultiData with non: operator+ class");
    }
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::operator-=(const multiData<T>& A)
  /*!
    Subtraction of other multiData
    \param A :: Dataset to subtract from this
  */
{
  if constexpr (std::is_arithmetic<T>::value)
    {
      throwMatchCheck(A,"operator-=");
  
      std::transform(flatData.begin(),flatData.end(),
		     A.flatData.begin(),flatData.begin(),
		     [](const T& a,const T& b) ->T
		     { return a-b; }
		     );
    }
  else
    {
      throw ColErr::AbsObjMethod
	("MultiData with non: operator-= class");
    }
  
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::operator*=(const multiData<T>& A)
  /*!
    Subtraction of other multiData
    \param A :: Dataset to subtract from this
  */
{
  if constexpr (std::is_arithmetic<T>::value)
    {
      throwMatchCheck(A,"operator*=");
      
      std::transform(flatData.begin(),flatData.end(),
		     A.flatData.begin(),flatData.begin(),
		     [](const T& a,const T& b) ->T
		     { return a*b; }
		     );
    }
  
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::operator/=(const multiData<T>& A)
  /*!
    Subtraction of other multiData
    \param A :: Dataset to subtract from this
  */
{
  if constexpr (std::is_floating_point<T>::value)
    {
      throwMatchCheck(A,"operator/=");
      
      std::transform(flatData.begin(),flatData.end(),
		     A.flatData.begin(),flatData.begin(),
		     [](const T& a,const T& b) ->T
		     { return (std::abs(b)>1e-38) ? a/b : a; }
		     );
    }
  else if constexpr (std::is_arithmetic<T>::value)
    {
      throwMatchCheck(A,"operator/=");
      
      std::transform(flatData.begin(),flatData.end(),
		     A.flatData.begin(),flatData.begin(),
		     [](const T& a,const T& b) ->T
		     { return (b!=0) ? a/b : a; }
		     );
    }
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::operator+=(const T& V)
  /*!
    Standard multiply operator
    \param V :: Value to multiply by
  */
{
  if constexpr (requires (T a,T b) { a+b; })
    {
      for(T& vItem : flatData)
	vItem+=V;
    }
  else
    {
      throw ColErr::AbsObjMethod
	("MultiData with non: operator+= class");
    }

  
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::operator-=(const T& V)   
  /*!
    Standard multiply operator
    \param V :: Value to multiply by
  */
{
  if constexpr (std::is_arithmetic<T>::value)
    {
      for(T& vItem : flatData)
	vItem-=V;
    }
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::operator*=(const T& V)
  /*!
    Standard multiply operator
    \param V :: Value to multiply by
  */
{  
  if constexpr (std::is_arithmetic<T>::value)
    {
      for(T& vItem : flatData)
	vItem*=V;
    }
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::operator/=(const T& V)
  /*!
    Standard division operator
    \param V :: Value to divide
  */
{
  if constexpr (std::is_floating_point<T>::value)
    {
      if (std::abs<T>(V)<1e-38)
	throw ColErr::NumericalAbort("multiData: Division value approx zero");
      
      for(T& vItem : flatData)
	vItem/=V;
    }
  else if constexpr (std::is_arithmetic<T>::value)
    {
      if (V!=0)
	throw ColErr::NumericalAbort("multiData: Division value zero");
      
      for(T& vItem : flatData)
	vItem/=V;
    }
  
  return *this;
}

template<typename T>
multiData<T>
multiData<T>::operator+(const multiData<T>& A) const
  /*!
    Addition of other multiData
    \param A :: Dataset to subtract from this
    \return datedataset
  */
{
  multiData<T> out(*this);
  out+=A;
  return out;
}

template<typename T>
multiData<T>
multiData<T>::operator-(const multiData<T>& A) const
  /*!
    Subtraction of other multiData
    \param A :: Dataset to subtract from this
    \return datedataset
  */
{
  multiData<T> out(*this);
  out-=A;
  return out;
}

template<typename T>
void
multiData<T>::getRangeImpl(std::vector<T>& out,
			   size_t& outIndex,
			   const std::vector<sRange>& sR,
			   const size_t dim,
			   const size_t index) const
   /*!
     Main worker function 
    */
{
  const sRange& RR=sR[dim];
  if (dim+1==sR.size())
    {
      for(size_t i=index+RR.begin;i<index+RR.end;i++)
	out[outIndex++]=flatData[i];
    }
  else
    {
      for(size_t i=RR.begin;i<RR.end;i++)
	getRangeImpl(out,outIndex,sR,dim+1,index+i*strides[dim]);
    }
  return;
}

template<typename T>
void
multiData<T>::getRangeSumImpl(T& out,
			      const std::vector<sRange>& sR,
			      const size_t dim,
			      const size_t index) const
   /*!
     Main worker function for integratoin
     \param out :: value to place data
     \param dim :: current dim to work on
     \param index :: current index to work on     y
    */
{
  if constexpr (requires (T a,T b) { a+b; })
    {
      const sRange& RR=sR[dim];
      if (dim+1==sR.size())
	{
	  for(size_t i=index+RR.begin;i<index+RR.end;i++)
	    out+=flatData[i];
	}
      else
	{
	  for(size_t i=RR.begin;i<RR.end;i++)
	    getRangeSumImpl(out,sR,dim+1,index+i*strides[dim]);
	}
    }
  return;
}

template<typename T>
std::vector<T>
multiData<T>::getFlatRange(std::vector<sRange> sR) const
  /*!
    Returns a vector (flat) that can be placed into a multiData
    unit as a range
    \param sR :: vector of ranges
    \return Ranged vector component
  */
{
  std::vector<T> out(getRangeSize(sR));

  size_t outIndex(0);
  getRangeImpl(out,outIndex,sR,0,0);
  return out;
}

template<typename T>
T
multiData<T>::getFlatIntegration(std::vector<sRange> sR) const
  /*!
    Returns a value from the sum of a range
    \param sR :: vector of ranges
    \return Ranged vector component
  */
{
  if constexpr (std::is_arithmetic<T>::value)
    {
      T outX(0);
      getRangeSumImpl(outX,sR,0,0);
      return outX;
    }
  throw ColErr::AbsObjMethod
    ("MultiData::getFlatIntegration with non: operator+ class");
 
}

template<typename T>
std::vector<T>
multiData<T>::getAxisRange(const size_t axisIndex,const size_t I) const
  /*!
    Returns a vector (flat) that can be placed into a multiData
    \param axisIndex :: axis (<nDim) to get effective nD-1 set from
    \param I :: Index from the given axisIndex to extract
    \return Ranged vector component
  */
{
  const size_t vSize=flatData.size()/index[axisIndex];

  std::vector<T> out(vSize);
  std::vector<sRange> sR;
  for(size_t i=0;i<index.size();i++)
    if (i!=axisIndex)
      sR.push_back(sRange({0,index[i]-1}));
    else
      sR.push_back(sRange({I,I}));

  size_t outIndex(0);
  getRangeImpl(out,outIndex,sR,0,0);
  return out;
}

template<typename T>
std::vector<T>
multiData<T>::getAxisIntegral(const size_t axisIndex,
			      sRange sUnit) const
  /*!
    Returns a vector (flat) that can be placed into a multiData
    of the integral along axisIndex
    \param axisIndex :: axis (<nDim) to get effective nD-1 set from
    \return Ranged vector component
  */
{
  if constexpr (std::is_arithmetic<T>::value)
    {
      const size_t vSize=flatData.size()/index[axisIndex];
      
      std::vector<sRange> sR;
      for(size_t i=0;i<index.size();i++)
	{
	  if (i!=axisIndex)
	    sR.push_back(sRange({0,index[i]-1}));
	  else
	    sR.push_back(sRange({0,0}));
	}
      
      if (!sUnit.end)
	sUnit.end=index[axisIndex];
      
      std::vector<T> out(vSize);
      std::vector<T> result(vSize);
      for(size_t i=sUnit.begin;i<sUnit.end;i++)
	{
	  sR[axisIndex]=sRange({i,i});
	  size_t outIndex(0);
	  getRangeImpl(out,outIndex,sR,0,0);
	  for(size_t j=0;j<vSize;j++)
	    result[j]+=out[j];
	}
      return result;
    }
  throw ColErr::AbsObjMethod
    ("MultiData::getAxisIntegral with non: operator+ class");

}

template<typename T>
multiData<T>
multiData<T>::exchange(size_t aIndex,size_t bIndex) const
 /*!
   Exchange index pattern with offsets aIndex / bIndex
   E.g. if [A][B][C][D] and a=>1 and b=>3 then
   result is [A][D][C][B]
   \param aIndex :: first column to exchange
   \param bIndex :: second column to exchange
  */
{
  if (aIndex>bIndex) std::swap(aIndex,bIndex);
  
  if (bIndex >= index.size() || aIndex==bIndex)
    throw ColErr::IndexError<size_t>
      (aIndex,bIndex,"exchange index not correct");


  std::vector<size_t> newIndex(index);
  std::swap(newIndex[aIndex],newIndex[bIndex]);

  multiData<T> Out(newIndex);
  std::vector<size_t> newStrides=strides;
  std::swap(newStrides[aIndex],newStrides[bIndex]);

  const size_t nA=size();      // full length
  const size_t nDim=getDim();  
  
  IndexCounter orgCNT(newIndex);
  
  std::vector<T>& newData=Out.flatData;
  for(size_t i=0;i<nA;i++)
    {
      size_t newIndex=0;
      for(size_t j=0;j<nDim;j++)
	newIndex+=orgCNT[j]*newStrides[j];

      newData[i]=flatData[newIndex];
      orgCNT++;
    }
  return Out;
}

template<typename T>
multiData<T>&
multiData<T>::exchangeIndex(const size_t indexA,const size_t indexB) 
 /*!
   Horrible function that transposes the index without
   changing the data 
  */
{
  if (indexA!=indexB && indexA<index.size() &&
      indexB<index.size())
    {
      std::vector<size_t> newIndex(index);
      std::swap(newIndex[indexA],newIndex[indexB]);
      setStrides(newIndex);
    }
  return *this;
}
      
template<typename T>
multiData<T>&
multiData<T>::transpose() 
 /*!
   Transpose the matrix so that [A][B][C]...
   goes to ..[C][B[A]
  */
{
  const size_t nDim(getDim());
  if (nDim>1)
    {
      std::vector<size_t> revIndex(index);
      IndexCounter<size_t> normCnt(revIndex.cbegin(),revIndex.cend());
      std::reverse(revIndex.begin(),revIndex.end());
      setStrides(revIndex);

      // copy as we are going to change current flat data
      const std::vector<T> oldData(flatData);
      const T* ptr=oldData.data();
      
      const std::vector<size_t>& RC=normCnt.getVector();
      do
	{
	  T* dataPtr=flatData.data();
	  for(size_t i=0; i<nDim;i++)
	    dataPtr+=RC[i]*strides[nDim-(i+1)];
	  *dataPtr= *ptr;
	  ptr++;
	} while(!normCnt++);

    }
  return *this;
}


template<typename T>
multiData<T>&
multiData<T>::transposeAxis(const size_t axisID) 
 /*!
   Transpose one axit of the matrix so that
   for Data[A][B][C]... and axisID=1
   and B of length N.
   
      Data[A][N-i][C]..
      \param azisID :: axis to use
  */
{
  if (axisID >= index.size())
    throw ColErr::IndexError<size_t>
      (axisID,index.size(),"transposeAxis index out of range");
  
  const size_t nDim(getDim());
  if (nDim==1)
    {
      std::reverse(flatData.begin(),flatData.end());
      return *this;
    }

  const size_t N=index[axisID];
  const size_t N2=N/2;

  std::vector<size_t> modIndex(index);
  modIndex[axisID]=1;
  IndexCounter<size_t> normCnt(modIndex.cbegin(),modIndex.cend());
  
  // copy as we are going to change current flat data

  const size_t axisStride=strides[axisID];
  const std::vector<size_t>& RC=normCnt.getVector();
  do 
    {
      T* dataPtrA=flatData.data();
      T* dataPtrB=flatData.data();
      for(size_t i=0; i<nDim;i++)
	{
	  if (i!=axisID)
	    {
	      dataPtrA+=RC[i]*strides[i];
	      dataPtrB+=RC[i]*strides[i];
	    }
	}
      dataPtrB+=(N-1)*axisStride;

      for(size_t j=0;j<N2;j++)
	{
	  std::swap(*dataPtrA,*dataPtrB);
	  dataPtrA+=axisStride;
	  dataPtrB-=axisStride;
	}
    } while(!normCnt++);
  return *this;
}

template<typename T>
std::vector<T>
multiData<T>::getAxisProjection(const size_t axisIndex) const
  /*!
    Returns a vector (flat) that can be placed into a multiData
    of the integral along axisIndex
    \param axisIndex :: axis (<nDim) to get effective nD-1 set from
    \return Ranged vector component
  */
{
  const size_t vSize=flatData.size()/index[axisIndex];
  std::vector<sRange> sR;
  for(size_t i=0;i<index.size();i++)
    if (i!=axisIndex)
      sR.push_back(sRange({0,index[i]-1}));
    else
      sR.push_back(sRange({0,0}));

  std::vector<T> result(index[axisIndex]);
  std::vector<T> out(vSize);  
  for(size_t i=0;i<index[axisIndex];i++)
    {
      sR[axisIndex]=sRange({i,i});
      result[i]=getFlatIntegration(sR);
    }
  return result;
}

template<typename T>
multiData<T>
multiData<T>::getRange(std::vector<sRange> sR) const
  /*!
    Returns a multiData object unit split down to the
    new range
    \param sR :: vector of ranges
    \return Ranged vector component
  */
{
  std::vector<T> out(getRangeSize(sR));

  size_t outIndex(0);
  getRangeImpl(out,outIndex,sR,0,0);
  std::vector<size_t> outArray;
  for(const sRange& sU : sR)
    {
      const size_t nUnits(sU.end-sU.begin);
      if (nUnits>=1)
	outArray.push_back(nUnits);
    }
  return multiData<T>(outArray,out);
}


template<typename T>
size_t
multiData<T>::getRangeSize(std::vector<sRange>& sR) const
  /*!
    Funciton to determine the size of the multiArray
    if fills in sR units that are empty with the full range

    It also forces sR to be bound by the actual index values
    
    \param sR :: range object
    \return flat size
   */
{
  size_t prod=1;
  for(size_t i=0;i<index.size();i++)
    {
      if (i>=sR.size())
	sR.push_back(sRange(0,index[i]));
      sRange& R=sR[i];
      if (R.begin>=index[i])
	{
	  R.begin=index[i]-1;
	  R.end=index[i];
	}
      else if (R.end>index[i])
	{
	  R.end=index[i];
	}
      else if (R.begin==R.end)  
	{
	  R.begin=0;
	  R.end=index[i];
	}

      prod*=(R.end-R.begin);
    }
  return prod;
}

template<typename T>
void
multiData<T>::resize(std::vector<long long unsigned int> newIndex)
  /*!
    Resize the data [make N-dimensional
    \param newIndex :: index list
   */
{
  std::vector<size_t> sIndex;
  for(const long long unsigned int I : newIndex)
    sIndex.emplace_back(static_cast<size_t>(I));
  resize(sIndex);
  return; 
}

template<typename T>
void
multiData<T>::resize(std::vector<size_t> newIndex)
  /*!
    Resize the data [make N-dimensional
    \param newIndex :: index list
   */
{
  if (newIndex != index)
    {
      index=std::move(newIndex);
      strides.resize(index.size()+1);
      strides[index.size()]=0;
      
      if(index.empty())
	{
	  flatData.clear();
	  return;
	}
      
      size_t prod=1;
      for(size_t i=index.size();i>0;i--)
	{
	  strides[i-1]=prod;
	  prod*=index[i-1];
	}
      
      if (index[0]*strides[0]!=flatData.size())
	flatData.resize(strides[0]*index[0]);
    }
  return; 
}

template<typename T>
void
multiData<T>::resize(const size_t A,const size_t B,
		     const size_t C,const size_t D)
  /*!
    Resize the data [make 3D]
    \param A :: first index
    \param B :: second index
    \param C :: third  index
    \param D :: fourth (outer) index
   */
{
  const size_t NSize(A*B*C*D);
  if (index.empty() || (index[0]*strides[0])!=NSize)
    flatData.resize(NSize);
  index={A,B,C,D};
  strides={B*C*D,C*D,D,1,0};

  return; 
}

template<typename T>
void
multiData<T>::resize(const size_t A,const size_t B,const size_t C)
  /*!
    Resize the data [make 3D]
    \param A :: first index
    \param B :: second index
    \param C :: third (outer) index
   */
{
  const size_t NSize(A*B*C);
  if (index.empty() || (index[0]*strides[0])!=NSize)
    flatData.resize(NSize);
  index={A,B,C};
  strides={B*C,C,1,0};

  return; 
}

template<typename T>
void
multiData<T>::resize(const size_t A,const size_t B)
  /*!
    Resize the data to a 2d array
    \param A :: first index
    \param B :: second index
   */
{
  const size_t NSize(A*B);
  if (index.empty() || (index[0]*strides[0])!=NSize)
    flatData.resize(NSize);
  index={A,B};
  strides={B,1,0};
  return; 
}

template<typename T>
void
multiData<T>::resize(const size_t A)
  /*!
    Resize the data to a 1d array
    \param A :: first index
   */
{
  const size_t NSize(A);
  if (index.empty() || (index[0]*strides[0])!=NSize)
    flatData.resize(NSize);
  index={A};
  strides={1,0};
  return; 
}

template<typename T>
void
multiData<T>::setData(std::vector<T> D)
  /*!
    Raw setting :
     Only sets if the stride number is correct
    \param D :: Vector to set

   */
{
  if (!index.empty())  // note: stride [index.size()+1]
    {
      if (D.size()>=strides[0]*index[0])
	flatData=std::move(D);
    }
  return;
}

template<typename T>
void
multiData<T>::setData(const size_t A,const size_t B,
		      const size_t C,
		      std::vector<T> Data)
  /*!
    Raw setting :
     Only sets if the stride number is correct
    \param Data :: Vector to set

   */
{
  if (A*B*C==Data.size())
    {
      index={A,B,C};
      strides={B*C,C,1,0};
      flatData=std::move(Data);
    }
  return;
}

template<typename T>
void
multiData<T>::setSubMap(const size_t axisIndex,
			const size_t unitIndex,
			const multiData<T>& A)
/*!
    Sets A to be in the map over the axisIndex at position
    unit index. A needs to be shape nDim-1 and have
    equal sizes to this.
  */
{
  if (axisIndex>=index.size() ||
      unitIndex>=index[axisIndex])
    throw ColErr::DimensionError<size_t>
      (index,A.index,"setSubMap:: dimension["+
       std::to_string(axisIndex)+":"+
       std::to_string(unitIndex));

  // IndexCounter IC<size_t>(index);
  // IndexCounter JC<size_t>(A.index());  

  // IC.setAxis(axisIndex,unitIndex);
  // const size_t NS(index.size());
  
  // for(

  
  return;
}


template<typename T>
std::vector<size_t>
multiData<T>::reduceAxis(const size_t axisIndex) const
  /*!
    Return a shape vector (index) without the axisIndex
    value (effectively move the shape to nDim-1
  */
{
  std::vector<size_t> out;
  
  for(size_t i=0;i<index.size();i++)
    if (i!=axisIndex)
      out.push_back(index[i]);
  return out;
}

template<typename T>
multiData<T>&
multiData<T>::increaseAxis(const size_t axisIndex) 
  /*!
    Increase the axis at the point index.
    i.e. [3][4][5] at index == 1 becomes
    [3][1][4][5]
    \param axisIndex :: insertion point of new axis
    \return size increased data
  */
{
  std::vector<size_t> newIndex;
  for(size_t i=0;i<index.size();i++)
    {
      if (i==axisIndex)
	newIndex.push_back(1);
      newIndex.push_back(index[i]);
    }
  if (newIndex.size()==index.size())
    newIndex.push_back(1);

  resize(newIndex);
  return *this;
}

template<typename T>
multiData<T>&
multiData<T>::reduce() 
  /*!
    Reduce out any size 1 dimentions
  */
{
  std::vector<size_t> newIndex;
  for(const size_t i : index)
    if (i!=1)
      newIndex.push_back(i);

  if (newIndex.size()!=index.size())
    resize(newIndex);
  return *this;
}

template<typename T>
multiData<T>
multiData<T>::reduceMap(const size_t axisIndex) const
  /*!
    Integrate across the map :
    Return a shape vector (index) without the axisIndex
    value (effectively move the shape to nDim-1

    \param axisIndex :: axis to remove 
  */
{
  std::vector<T> intData=getAxisIntegral(axisIndex);
  return multiData<T>(reduceAxis(axisIndex),intData);
}

template<typename T>
multiData<T>&
multiData<T>::cut(const size_t axisIndex,
		  const size_t itemIndex) 
/*!
    Integrate across the map :
    Return a shape vector (index) without the axisIndex
    value (effectively move the shape to nDim-1

    \param axisIndex :: axis to remove
    \param itemIndex :: component of axis to use
  */
{
  if (axisIndex>=index.size())
    throw ColErr::IndexError<size_t>
      (axisIndex,index.size(),"axisIndex out of dimenstion");
  if (itemIndex>=index[axisIndex])
    throw ColErr::IndexError<size_t>
      (itemIndex,index[axisIndex],"itemIndex out of shape range");

  // special case for no data
  if (index.size()==1)
    {
      index.clear();
      strides={0};
      flatData.clear();
      return *this;
    }
  
  std::vector<T> intData=getAxisRange(axisIndex,itemIndex);

  index.erase(index.begin()+axisIndex);
  strides.resize(index.size()+1);
  strides[index.size()]=0;
  size_t prod=1;
  for(size_t i=index.size();i>0;i--)
    {
      strides[i-1]=prod;
      prod*=index[i-1];
    }
  
  flatData=std::move(intData);

  return *this;
}

template<typename T>
void
multiData<T>::combine(const size_t axisA,
		      const size_t axisB) 
  /*!
    Combine two rows into the first. Preserves the size
    of the vector. Index B is interleaved between index A
    and index B
  */
{
  if (!flatData.empty())
    {
      if (axisA>=index.size() ||
	  axisB>=index.size()  ||
	  axisA==axisB)
	throw ColErr::IndexError<size_t>
	  (axisA,axisB,"combine not in range range");

      std::vector<size_t> newIndex;

      // size_t primeA;
      // size_t primeB;
      for(size_t i=0;i<index.size();i++)
	{
	  if (i==axisA)
	    {
	      //	      primeA=newIndex.size();
	      newIndex.push_back(index[i]*index[axisB]);
	    }
	  else if (i!=axisB)
	    {
	      //      primeB=newIndex.size();
	      newIndex.push_back(index[i]);
	    }
	}
      if (axisB>axisA && axisB-axisA==1)
	{
	  this->resize(newIndex);
	  return;
	}
    }
  return;
}

template<typename T>
multiData<T>
multiData<T>::projectMap(const size_t axisIndex,
			 std::vector<sRange> sR) const
  /*!
    Return a map with shape 1, size index[axisIndex]
    with integral of the range in Sr
    \parma axisIndex :: Primary index
  */
{
  if constexpr (std::is_arithmetic<T>::value)
    {
      getRangeSize(sR);
      // single vector
      const size_t nR=(!sR[axisIndex].end) ?
	index[axisIndex] : sR[axisIndex].end-sR[axisIndex].begin;
      const size_t nEnd=sR[axisIndex].end;
      
      std::vector<T> outVec(nR);
      size_t index(0);
      for(size_t i=sR[axisIndex].begin;i<nEnd;i++)
	{
	  sR[axisIndex]=sRange({i,i});
	  outVec[index++]=getFlatIntegration(sR);
	}
      return multiData<T>(nR,outVec);
    }
  throw ColErr::AbsObjMethod
    ("MultiData::getAxisIntegral with non: operator+ class");
}

template<typename T>
multiData<T>
multiData<T>::integrateMap(const size_t axisIndex,
			   std::vector<sRange> sR) const
  /*!
    Return a multiData with shape nDim-N ( intIndex=set )
    with integral of the vector give by SR
    \param axisIndex :: axis to remove
  */
{
  if constexpr (std::is_arithmetic<T>::value)
    {
      multiData<T> outI=getRange(std::move(sR));
      return multiData<T>(outI.reduceAxis(axisIndex),
			  outI.getAxisIntegral(axisIndex));
    }
  throw ColErr::AbsObjMethod
    ("MultiData::integrateMap with non: operator+ class");
}

template<typename T>
T
multiData<T>::integrateValue(std::vector<sRange> sR) const
  /*!
    Return a value of the integrand
    \param sR :: range ot use
    \return sum of range
  */
{
  if constexpr (std::is_arithmetic<T>::value)  
    {
      std::vector<T> values=getFlatRange(std::move(sR));
      T Out(0);
      for(const T& v : values)
	Out+=v;
      return Out;
    }
  throw ColErr::AbsObjMethod
    ("MultiData::IntegrateValue with non: operator+ class");
  
}

template<typename T>
size_t
multiData<T>::offset(const size_t A,const size_t B) const
  /*!
    Return offset points index value at A,B 
  */
{
  const T* ptr=get()[A][B].pointer();
  return static_cast<size_t>(ptr-getPtr()); 
}

template<typename T>
size_t
multiData<T>::offset(const size_t A,const size_t B,const size_t C) const
  /*!
    Return offset points [specialized for 3 index systems]
    \param A :: first index
    \param B :: second index
    \param C :: third index
   */
{
  const T* ptr=get()[A][B][C].pointer();
  return static_cast<size_t>(ptr-getPtr()); 
}

template<typename T>
size_t
multiData<T>::offset(const std::vector<size_t>& VI) const
  /*!
    Return offset points index value at VI.a[
  */
{
  sliceUnit<const T> Item=get();
  for(const size_t I : VI)
    Item=Item[I];

  const T* ptr=Item.pointer();
  return static_cast<size_t>(ptr-getPtr()); 
}

template<typename T>
T&
multiData<T>::value(const std::vector<size_t>& VI) 
  /*!
    Return offset points index value at VI.a[
    \param VI :: Index
  */
{
  T* dataPtr=flatData.data();
  for(size_t i=0; i+1<strides.size() && i<VI.size();i++)
    dataPtr+=VI[i]*strides[i];
  
  return *dataPtr;
}

template<typename T>
const T&
multiData<T>::value(const std::vector<size_t>& VI) const
  /*!
    Return offset points index value at VI.a[
    \param VI :: Index
  */
{
  const T* dataPtr=flatData.data();
  for(size_t i=0; i+1 <strides.size() && i<VI.size();i++)
    dataPtr+=VI[i]*strides[i];
  
  return *dataPtr;  
}

template<typename T>
void
multiData<T>::fill(const T& V) 
  /*!
    Fill the data set with a value
    \param V :: Value
  */
{
  std::fill(flatData.begin(),flatData.end(),V);
  return;
}


template<typename T>
std::string
multiData<T>::simpleStr() const
  /*!
    Write out the shape in a simple way
  */
{
  std::string out("[");
  out+=std::to_string(flatData.size())+"] ";
  for(const size_t i : index)
    out+=std::to_string(i)+" ";
  return out;
}

template<typename T>
void
multiData<T>::write(std::ostream& OX) const
  /*!
    Write out the matrix in a nice way
    \param OX :: Output
  */
{
  if constexpr (requires (std::ostream& OX,T A) { OX<<A; })
    {
      OX<<"Index =";
      for(const size_t i : index)
	OX<<" "<<i;
      OX<<"\n";
      const size_t nDim=index.size();
     
      if (nDim>1)
	{
	  typename std::vector<T>::const_iterator vc(flatData.begin());
	  IndexCounter<size_t> primeIndex(index.begin(),index.end()-2);
	  do
	    {
	      if (nDim>2)
		OX<<"Mat"<<primeIndex<<"\n";
	      for(size_t j=0;j<index[nDim-2];j++)
		{
		  OX<<"   ";
		  for(size_t k=0;k<index.back();k++)
		    OX<<" "<<*vc++;
		  OX<<"\n";
		}
	    } while(!primeIndex++);
	}
      else if (nDim==1)
	{
	  OX<<"   ";
	  for(const T& v : flatData)
	    OX<<" "<<v;
	  OX<<"\n";
	}
    }
  return;
}

///\cond TEMPLATE

namespace delftSystem
{
  class RElement;
}

template class multiData<double>;
template class multiData<DError::doubleErr>;
template class multiData<float>;
template class multiData<std::string>;
template class multiData<int>;
template class multiData<size_t>;
template class multiData<std::shared_ptr<delftSystem::RElement>>;

template multiData<double>::multiData(const multiData<float>&);
template multiData<float>::multiData(const multiData<double>&);
template multiData<float>::multiData(const size_t,const std::vector<double>&);
template multiData<double>::multiData(const size_t,const std::vector<float>&);

template std::ostream& operator<<(std::ostream&,const multiData<int>&);
template std::ostream& operator<<(std::ostream&,const multiData<float>&);
template std::ostream& operator<<(std::ostream&,const multiData<std::string>&);
template std::ostream& operator<<(std::ostream&,const multiData<size_t>&);

///\endcond TEMPLATE
