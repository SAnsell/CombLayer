#include <iostream>
#include <cmath>
#include <complex>
#include <vector>

#include "Exception.h"
#include "MapRange.h"

namespace MapSupport
{

template<typename T>
Range<T>::Range(const T& V) :
  low(V),high(V)
  /*!
    Constructor
    \param V :: Single Value to set 
  */
{}

template<typename T>
Range<T>::Range(const T& LV,const T& HV) :
  low(LV),high(HV)
  /*!
    Constructor
    \param LV :: Low values [not check]
    \param HV :: High values [not check]
  */
{}

template<typename T>
bool
Range<T>::overlap(const Range<T>& A,
		  const T& proximity) const
  /*!
    Determine if a range overlaps
    \param A :: Range to check
    \param proximity :: vlaue 
  */
{
  if ((A.low-proximity <= high)
      && A.high+proximity >=high)
    return 1;
  
  if ((A.high+proximity >= low)
      && A.low-proximity <= low)
    return 1;

  return 0;
}

template<typename T>

Range<T>&
Range<T>::combine(const Range<T>& A) 
  /*!
    Combine the biggest mixed range
    \param A :: Range to combine
    \return new Range
  */
{
  low=std::min(low,A.low);
  high=std::max(high,A.high);

  return *this;
}


/// \cond TEMPLATE

template class Range<int>;

/// \endcond TEMPLATE

} // NAMESPACE MapSupport
