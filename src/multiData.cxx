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
#include <type_traits>

#include "Exception.h"
#include "doubleErr.h"
#include "IndexCounter.h"
#include "dataSlice.h"
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
    \param B :: size of first item
    \param C :: size of first item
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
  if constexpr (std::is_arithmetic<T>::value)
    {
      throwMatchCheck(A,"operator/=");
      
      std::transform(flatData.begin(),flatData.end(),
		     A.flatData.begin(),flatData.begin(),
		     [](const T& a,const T& b) ->T
		     { return (std::abs(b)>1e-38) ? a/b : a; }
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
  if constexpr (std::is_arithmetic<T>::value)
    {
      if (std::abs(V)<1e-38)
	throw ColErr::NumericalAbort("multiData: Division value approx zero");
      
      for(T& vItem : flatData)
	vItem/=V;
    }
  
  return *this;
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
    \param A :: second index
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
multiData<T>
multiData<T>::reduceMap(const size_t axisIndex) const
  /*!
    Return a shape vector (index) without the axisIndex
    value (effectively move the shape to nDim-1
  */
{
  std::vector<T> intData=getAxisIntegral(axisIndex);
  return multiData<T>(reduceAxis(axisIndex),intData);
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
      
      std::vector<T> outVec(nR);
      size_t index(0);
      for(size_t i=sR[axisIndex].begin;i<nR;i++)
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
    Return a vector with shape nDim-N ( intIndex=set )
    with integral of the vector
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
    ("MultiData::getAxisIntegral with non: operator+ class");
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
      
      typename std::vector<T>::const_iterator vc(flatData.begin());
      
      if (nDim>1)
	{
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
	  for(size_t j=0;j<strides[0];j++)
	    OX<<" "<<*vc++;
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
template class multiData<std::shared_ptr<delftSystem::RElement>>;

template std::ostream& operator<<(std::ostream&,const multiData<int>&);
template std::ostream& operator<<(std::ostream&,const multiData<float>&);
template std::ostream& operator<<(std::ostream&,const multiData<std::string>&);

///\endcond TEMPLATE
