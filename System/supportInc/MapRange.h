/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/MapRange.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef MapSupport_MapRange_h
#define MapSupport_MapRange_h

namespace MapSupport
{
/*!
  \class Range
  \author S. Ansell
  \date May 2015
  \version 1.0
  \brief Range object to allow non-overlapping ranged maps
  \todo update to have an inserter and amalgomator
*/

template<typename T>
class Range
{
 private:

  T low;           ///< Low value
  T high;          ///< High value

public:

  /// constructor
  explicit Range(const T&);
  Range(const T&,const T&);

  /// Comparitor operator
  bool  operator<(const Range<T>& A) const
    { return  (high < A.low) ? 1 : 0; }
  /// Equal operator
  bool operator==(const Range<T>& A) const
    { return  (low==A.low && high==A.high) ? 1 : 0; }

  /// Inequality operator
  bool operator!=(const Range<T>& A) const
  { return  !(this->operator==(A)); }

  bool valid(const T&) const;
  bool overlap(const Range<T>&,const T& =T(0)) const;
  Range<T>& combine(const Range<T>&);
  void write(std::ostream&) const;
}; 

template<typename T>
std::ostream&
operator<<(std::ostream&,const Range<T>&);

} // NAMESPACE MapSupport


#endif
