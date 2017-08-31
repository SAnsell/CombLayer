/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/mathSupport.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef mathSupport_h
#define mathSupport_h

/*!
  \struct IncSeq
  \brief Allows a simple increment to take place
  \author S. Ansell
  \version 1.0
  \date September 2012
*/
struct IncSeq{ 
  size_t v;                          ///< Value 
  size_t operator()(){return v++;}   ///< increment
IncSeq():v(0){}                      ///< Constructor
};

size_t lowBitIndex(const unsigned int&);
size_t lowBitIndex(const size_t&);
size_t countBits(const size_t&);
size_t countBits(const unsigned int&);
long int factorial(const int);       
double factorialDB(const int);
double fibonacci(const int);
double normalDist(const double); ///< convert a number 0->1 into a normal distribute
double invErf(const double);    ///< Inverse error function

template<typename T>
int 
sign(const T& V)
  /*!
    Determine the sign of an object with zero being 
    treated specially
    \param V :: Value to test
    \return -1 : 0 : 1 (dependend of value)
  */
{
  if (V<0) return -1;
  if (V>0) return 1;
  return 0;
}

template<typename T>
long int
indexPos(const std::vector<T>&,const T&);

/// Range and initialise polynominal calculation
template<typename TX,typename TY>
TY
polInterp(const TX&,const unsigned int,const std::vector<TX>&,
	  const std::vector<TY>&);


/// Calcluate a polynominal fit to a segment of data
template<typename TX,typename TY>
TY polFit(const TX&,const unsigned int,
  	  typename std::vector<TX>::const_iterator,
  	  typename std::vector<TY>::const_iterator);

/// Determine closes point to aim in a vector
template<typename T> 
typename std::vector<T>::const_iterator
iteratorPos(const std::vector<T>&,const T&);

/// Integrate on irregular grid using quadratic
template<typename T>
T intQuadratic(const typename std::vector<T>::const_iterator&,
	       const typename std::vector<T>::const_iterator&);

/// Integrate on irregular grid using quadratic
template<typename T>
T intQuadratic(const typename std::vector<T>::const_iterator&,
	       const typename std::vector<T>::const_iterator&);

template<typename T>
T derivQuadratic(const typename std::vector<T>::const_iterator&,
		 const typename std::vector<T>::const_iterator&);
template<typename T>
T d2dxQuadratic(const typename std::vector<T>::const_iterator&,
		const typename std::vector<T>::const_iterator&);

template<typename T>
T norm(const std::vector<T>&);


 
/// Create an index of the vector in sorted order.
template<typename T,typename U> 
void indexSort(const std::vector<T>&,std::vector<U>&);


/*!
  \namespace mathFunc
  \brief Holds simple functions for numerical stuff 
  \version 1.0
  \author S. Ansell
  \date January 2006
  
  Holds new math functions, we will put the above stuff in
  it later (hopefully)
*/
namespace mathFunc
{
  template<typename T> void Order(T&,T&);  ///< Simple ordering of two components
  template<typename T> void Swap(T&,T&);   ///< Simple exchange of two components
  template<typename T,typename U> void crossSort(std::vector<T>&,std::vector<U>&);
  template<typename T> 
  long int binSearch(const typename std::vector<T>::const_iterator&,
		const typename std::vector<T>::const_iterator&,
		const T&);
  /// Make a vector random
  template<typename T> void randomize(std::vector<T>&);

  /// Make a vector random
  template<template<typename T,typename Alloc>  class V,typename T,typename Alloc> 
    T minDifference(const V<T,Alloc>&,const T&);
  //  template<typename LTYPEtypename T> T minSeperation(const LTYPE<T>&);

  template<typename T>
  std::vector<T> filter(const std::vector<T>&,const size_t);

  template<typename T>
  T mean(const std::vector<T>&);

  double logAdd(const double&,const double&);
  double logSubtract(const double&,const double&);

  double logFromLinear(const double,const double,const size_t,
		      const size_t);
  
}


/*!
  \namespace mathSupport
  \brief Holds functors to do numerical operations
  \version 1.0
  \author S. Ansell
  \date August 2005
  
  Holds all the numberical operations for comparison
  and checking needed for containers of objects.
*/

namespace mathSupport
{

  /*!
    \struct Rsol
    \brief A simple imagenary class (replace by complex)
    \version 1.0
    \author S. Ansell
  */
struct Rsol
{
  double re;    ///< real value
  double im;    ///< imag value
};

/*!
  \class PIndex 
  \author S. Ansell
  \date Aug 2005
  \version 1.0
  \brief Class  to fill an index with a progressive count
*/

template<typename T>
class PIndex
{
private:

  size_t count;    ///< counter

public:

  /// Constructor
  PIndex() : count(0) { }

  /// functional
  std::pair<T,size_t> operator()(const T& A) 
  {  return std::pair<T,size_t>(A,count++); }

};

/*!
  \class PCombine
  \brief Combines two values into a  pair
  \author S. Ansell
  \date August 2006
  \version 1.0
*/

template<typename T,typename U>
class PCombine
{
public:

  /// Combination operator
  std::pair<T,U> operator()(const T& A,const U& B) 
    {  return std::pair<T,U>(A,B); }

};

/*!
  \class PSep 
  \author S. Ansell
  \date Aug 2005
  \version 1.0
  \brief Class to access the second object in index pair. 
*/
template<typename T>
class PSep
{ 
public:

  /// Functional to the second object
  size_t operator()(const std::pair<T,size_t>& A) 
    { return A.second; }

};

/*!
  \class PairFstEq
  \brief Functonal to provide equal for the first object in a pair.
  \author S. Ansell
  \date February 2006
  \version 1.0
*/

template<typename T,typename U>
 class PairFstEq : public  std::binary_function<std::pair<T,U>,T, bool >     //Note this needs to be non-constant
  {
    public:

    /// Check first items
      bool operator()(const std::pair<T,U>& A,const T& B) const
	{ 
	  return (A.first==B);
	}
  };

/*!
  \class PairSndEq
  \brief Functonal to provide equal for the second object in a pair.
  \author S. Ansell
  \date January 2006
  \version 1.0
*/

template<typename T,typename U>
 class PairSndEq : public  std::binary_function<std::pair<T,U>,U, bool >     //Note this needs to be non-constant
  {
    public:

      /// Check only second items
      bool operator()(const std::pair<T,U>& A,const U& B) const
	{ 
	  return (A.second==B);
	}
  };

/*!
  \class PairFstLess
  \brief Functonal to provide less than for the first object in a pair.
  \author S. Ansell
  \date June 2006
  \version 1.0
*/

template<typename T,typename U>
 class PairFstLess : 
 public  std::binary_function<std::pair<T,U>,std::pair<T,U>,bool >     //Note this needs to be non-constant
  {
    public:
    
    /// Direct comparison operator
    bool operator()(const std::pair<T,U>& A, const std::pair<T,U>& B) const    
      { 
	return (A.first<B.first);
      }
    
    /// Subsequent comparison operator
    bool operator()(const std::pair<T,U>& A, const T& B) const
	{ 
	  return (A.first<B);
	}
    };

/*!
  \class PairSndLess
  \brief Functonal to provide lessthan for the second object in a pair.
  \author S. Ansell
  \date January 2006
  \version 1.0
*/

template<typename T,typename U>
 class PairSndLess : 
 public  std::binary_function<std::pair<T,U>,std::pair<T,U>,bool >     //Note this needs to be non-constant
  {
    public:
    
    /// Direct comparison operator
    bool operator()(const std::pair<T,U>& A, const std::pair<T,U>& B) const    
      { 
	return (A.second<B.second);
      }
    
    /// Subsequent comparison operator
    bool operator()(const std::pair<T,U>& A, const U& B) const
	{ 
	  return (A.second<B);
	}
    };



/*!
  \class PSortBoth
  \brief Class to sort on the second object then first obj
  \author S. Ansell
  \date April 2005
  \version 1.0

*/
template<typename T,typename U>
class PSortBoth
{
public:

  /// Sort functional
  bool operator()(const std::pair<T,U>& A,const std::pair<T,U>& B) 
    { 
      return (A.second==B.second) ?
	A.first>B.first :  A.second>B.second; 
    }
};

/*!
  \class PSortSecond
  \brief Class to sort on the second object
  \author S. Ansell
  \date January 2006
  \version 1.0

*/
template<typename T,typename U>
class PSortSecond
{
public:

  /// Sort functional
  bool operator()(const std::pair<T,U>& A,const std::pair<T,U>& B) 
    { 
      return A.second<B.second;
    }
};

/*!
  \class absComp
  \brief Class to compare absolute numbers 
  \author S. Ansell
  \date September 2005
  \version 1.0
*/

template<typename T>
class absComp
{
public:

  /// Does the absolute comparison
  bool operator()(const T& A,const T& B) const
    {
      return (std::abs(A)<std::abs(B));
    }

};

/*!
  \class tolEqual
  \brief Class to compare near numbers
  \author S. Ansell
  \date December 2010
  \version 1.0
*/
class tolEqual
{
 private:

  const double tol;        ///< Tolerance

 public:
  /// constructor
  tolEqual(const double D) : tol(D) {}
  /// Comparison
  bool operator()(const double& A,const double& B)  const
    {
      return (std::abs(A-B)<tol) ? 1 : 0;
    }

};


} // NAMESPACE mathSupport
#endif

