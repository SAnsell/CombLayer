/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/IndexCounter.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef IndexCounter_h
#define IndexCounter_h

/*!
  \class IndexCounter
  \version 1.0
  \date September 2005
  \author S. Ansell
  \brief Simple multilevel-cyclic counter

  Objective is a rolling integer stream ie 1,2,3
  going to 1,2,N-1 and then 1,3,4 etc...
*/

template<typename T>
class IndexCounter
{
 private:

  std::vector<T> Rmax;         ///< Numbers to over cycle
  std::vector<T> RC;           ///< rotation list

 public:

  template<typename InputIter>
  IndexCounter(InputIter,InputIter);              
  IndexCounter(std::vector<T>);              
  IndexCounter(const size_t,const T&);
  IndexCounter(const T&,const T&,const T&);  
  IndexCounter(const IndexCounter&);
  IndexCounter(IndexCounter&&);
  IndexCounter& operator=(const IndexCounter&);
  IndexCounter& operator=(IndexCounter&&);
  ~IndexCounter() =default;

  bool operator==(const IndexCounter<T>&) const;
  bool operator<(const IndexCounter<T>&) const;
  bool operator>(const IndexCounter<T>&) const;
  /// Accessor operator
  T operator[](const size_t I) const { return RC[I]; }

  bool operator++();
  bool operator++(const int);
  bool operator--();
  bool operator--(const int);

  void setSingleMax(const T&);
  /// access to vector
  const std::vector<T>& getVector() const { return RC; }
  void write(std::ostream&) const;
};

template<typename T>
std::ostream& operator<<(std::ostream&,const IndexCounter<T>&);


#endif
