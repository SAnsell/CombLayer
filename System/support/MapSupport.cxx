#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <map>

#include "Exception.h"
#include "MapSupport.h"

namespace MapSupport
{

template<typename T>
bool
iterateBinMap(typename std::map<T,int>& M)
  /*!
    Simple iteratator over a 0/1 state mpap
   */
{
  for(typename std::map<T,int>::value_type& mc : M)
    {
      if (mc.second)
	{
	  // zero and move to next
	  mc.second=0;  
	}
      else
	{
	  mc.second=1;
	  return 0;  // no more work to do
	}
    }
  return 1;
}

template 
bool iterateBinMap(std::map<int,int>&);
  
} // NAMESPACE MapSupport
