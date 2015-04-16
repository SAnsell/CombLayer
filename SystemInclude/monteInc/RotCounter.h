/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/RotCounter.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef RotaryCounter_h
#define RotaryCounter_h

/*!
  \class RotaryCounter
  \version 1.0
  \date September 2005
  \author S. Ansell
  \brief Simple multilevel-cyclic counter

  Objective is a rolling integer stream ie 1,2,3
  going to 1,2,N-1 and then 1,3,4 etc...
*/

template<typename T>
class RotaryCounter
{
 private:

  T Rmax;                 ///< Number to over cycle
  std::vector<T> RC;        ///< rotation list

 public:
  
  RotaryCounter(const size_t,const T&);  ///<Size,Max
  RotaryCounter(const RotaryCounter&);
  RotaryCounter& operator=(const RotaryCounter&);
  ~RotaryCounter();

  bool operator==(const RotaryCounter&) const;
  bool operator<(const RotaryCounter&) const;
  bool operator>(const RotaryCounter&) const;
  /// Accessor operator
  T operator[](const size_t I) const { return RC[I]; }
  bool operator++();
  bool operator++(const int);
  bool operator--();
  bool operator--(const int);
  
  void write(std::ostream&) const;
};

template<typename T>
std::ostream& operator<<(std::ostream&,const RotaryCounter<T>&);


#endif
