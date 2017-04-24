/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/cycIterator.h
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


#ifndef cycIterator_h
#define cycIterator_h

/*!
  \class cycIterator
  \tparam  IT :: Iterator type 
  \brief Iterator to loop round a sets
  \author Stuart Ansell
  \date April 2015
*/

template<typename T,typename Iterator> 
class cycIterator : 
  public std::iterator
    <std::bidirectional_iterator_tag, T,std::ptrdiff_t>
{
 private:

  int loopIndex;        ///< loop count
  Iterator beginIter;   ///< Begin point
  Iterator endIter;     ///< End point 
  Iterator vc;          ///< Current position  

 public:

  cycIterator(const Iterator&,const Iterator&);
  cycIterator(const cycIterator<T,Iterator>&);
  /// iterator assignment operator
  cycIterator<T,Iterator>& operator=(const Iterator& A)
    { loopIndex=0; vc=A; return *this; }
  cycIterator<T,Iterator>& operator=(const cycIterator<T,Iterator>&); 
  ~cycIterator() {}   ///< Destructor

  /// access base object
  const T& operator*() const { return *vc; }
  cycIterator<T,Iterator>& operator++();
  cycIterator<T,Iterator> operator++(int);
  /// access loop counter
  int getLoop() const { return loopIndex; }

};




#endif
