#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <iterator>

#include "Exception.h"
#include "cycIterator.h"

namespace RMCbox
{
  class Atom;
}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>::cycIterator(const Iterator& A,const Iterator& B) :
  loopIndex(0),beginIter(A),endIter(B),vc(A)
  /*!
    constructor
    \param A :: Iterator [start]
    \param B :: Iterator [end] 
  */
{}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>::cycIterator(const cycIterator<T,Iterator>& A) : 
  loopIndex(A.loopIndex),beginIter(A.beginIter),
  endIter(A.endIter),vc(A.vc)
  /*!
    Copy constructor
    \param A :: Copy object
  */
{}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>&
cycIterator<T,Iterator>::operator=(const cycIterator<T,Iterator>& A) 
  /*!
    Assignment operator
    \param A :: Copy object
    \return Object
  */
{
  if (this!=&A)
    {
      loopIndex=A.loopIndex;
      beginIter=A.beginIter;
      endIter=A.endIter;
      vc=A.vc;
    }
  return *this;
}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>
cycIterator<T,Iterator>::operator++(int) 
  /*!
    Simple addor
   */
{
  cycIterator<T,Iterator> out(*this);
  ++*this;
  return out;
}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>&
cycIterator<T,Iterator>::operator++()
  /*!
    Simple addition operator [pre adder]
    \return iterator
  */
{
  vc++;
  if (vc==endIter)
    {
      loopIndex++;
      vc=beginIter;
    }
  return *this;
}

/// \cond TEMPLATE

template class cycIterator<RMCbox::Atom*,
			   std::vector<RMCbox::Atom*>::const_iterator>;
  // template class Matrix<int>;

/// \endcond TEMPLATE
