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

#include "Exception.h"
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
  index(std::move(I)),strides(index.size())
  /*!
    Constructor with full size
    with array index and data
  */
{
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
multiData<T>::multiData(std::vector<size_t> I,const std::vector<T> D) :
  index(std::move(I)),strides(index.size()),
  flatData(std::move(D))
  /*!
    Constructor with full size
    with array index and data
  */
{
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
  index({A}),strides({1}),
  flatData(A)
  /*!
    Constructor with full size (1D)
    \param A :: size of first item
  */
{}

template<typename T>
multiData<T>::multiData(const size_t A,const size_t B) :
  index({A,B}),strides({B,1}),
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
  index({A,B,C}),strides({B*C,C,1}),
  flatData(A*B*C)
  /*!
    Constructor with full size
    \param A :: size of first item
    \param B :: size of first item
    \param C :: size of first item
  */
{}


template<typename T>
multiData<T>::multiData(const size_t A,std::vector<T> D) :
  index({A}),strides({1}),
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
  index({A,B}),strides({B,1}),
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
  index({A,B,C}),strides({B*C,C,1}),
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
  if (index.size()==3)
    std::cout<<"COPY == "<<index[0]<<" "<<index[1]<<" "<<index[2]<<std::endl;
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
      if (index.size()==3)
	std::cout<<"OPER == "<<index[0]<<" "<<index[1]<<" "<<index[2]<<std::endl;
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
  throwMatchCheck(A,"operator+=");
  
  std::transform(flatData.begin(),flatData.end(),
		 A.flatData.begin(),flatData.begin(),
		 [](const T& a,const T& b) ->T
		 { return a+b; }
		 );
  
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
  throwMatchCheck(A,"operator-=");
  
  std::transform(flatData.begin(),flatData.end(),
		 A.flatData.begin(),flatData.begin(),
		 [](const T& a,const T& b) ->T
		 { return a-b; }
		 );
  
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
  throwMatchCheck(A,"operator-=");
  
  std::transform(flatData.begin(),flatData.end(),
		 A.flatData.begin(),flatData.begin(),
		 [](const T& a,const T& b) ->T
		 { return a-b; }
		 );
  
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
  
  for(T& vItem : flatData)
    vItem*=V;
  
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

  if (std::abs(V)<1e-38)
    throw ColErr::NumericalAbort("multiData: Division value approx zero");
  
  for(T& vItem : flatData)
    vItem/=V;
  
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
multiData<T>::getAxisIntegral(const size_t axisIndex) const
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

  std::vector<T> out(vSize);
  std::vector<T> result(vSize);
  for(size_t i=0;i<index[axisIndex];i++)
    {
      sR[axisIndex]=sRange({i,i});
      size_t outIndex(0);
      getRangeImpl(out,outIndex,sR,0,0);
      for(size_t j=0;j<vSize;j++)
	result[j]+=out[j];
    }
  return result;
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
      size_t outIndex(0);	    
      getRangeImpl(out,outIndex,sR,0,0);
      for(size_t j=0;j<vSize;j++)
	result[i]+=out[j];
    }
  return result;
}

template<typename T>
multiData<T>
multiData<T>::getRange(std::vector<sRange> sR) const
  /*!
    Returns a multiData object  unit as a range
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
  index=std::move(newIndex);
  strides.resize(index.size());

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
  strides={B*C,C,1};

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
  strides={B,1};
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
  if (!strides.empty())
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
    Return a amp with shape 1, size index[axisIndex]
    with integral of the range in Sr
  */
{
  multiData<T> rI=getRange(std::move(sR));

  std::vector<T> outVec=
    rI.getAxisProjection(axisIndex);

  return multiData<T>(outVec.size(),outVec);
}

template<typename T>
multiData<T>
multiData<T>::integrateMap(const size_t axisIndex,
			   std::vector<sRange> sR) const
  /*!
    Return a vector with shape nDim-N ( intIndex=set )
    with integral  of 
  */
{
  multiData<T> outI=getRange(std::move(sR));
  return multiData<T>(outI.reduceAxis(axisIndex),
		      outI.getAxisIntegral(axisIndex));
}

template<typename T>
size_t
multiData<T>::offset(const size_t A,const size_t B) const
  /*!
    Return offset points index value at A,B 
  */
{
  const T* ptr=get()[A][B].pointer();
  return ptr-getPtr(); 
}



template<typename T>
size_t
multiData<T>::offset(const size_t A,const size_t B,const size_t C) const
  /*!
    Return offset points
   */
{
  const T* ptr=get()[A][B][C].pointer();
  return ptr-getPtr(); 
}

template<typename T>
void
multiData<T>::write(std::ostream& OX) const
{
  OX<<"Index =";
  for(const size_t i : index)
    OX<<" "<<i;
  return;
}

///\cond TEMPLATE
template class multiData<double>;
template class multiData<float>;
template class multiData<int>;
template std::ostream& operator<<(std::ostream&,const multiData<int>&);
template std::ostream& operator<<(std::ostream&,const multiData<float>&);

///\endcond TEMPLATE
