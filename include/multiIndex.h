/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/multiIndex.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
#ifndef multiIndex_h
#define multiIndex_h

/*!
  \class multiIndex
  \version 1.0
  \date September 2005
  \author S. Ansell
  \brief Fixed component counter for index groups

  Objective is a rolling integer 

*/
class multiIndex
{
 private:

  size_t currentIndex;               ///< current index point
  std::vector<size_t> stride;        ///< actual stride values

  std::vector<size_t> Rmin;         ///< Numbers to over cycle
  std::vector<size_t> Rmax;         ///< Numbers to over cycle
  std::vector<size_t> RC;           ///< rotation list

  size_t calcIndex() const;
  
 public:


  multiIndex(const size_t,const size_t);
  multiIndex(const size_t,const size_t,const size_t);
  multiIndex(const size_t,const size_t,const std::vector<size_t>&);          
  multiIndex(multiIndex&&) =default;
  multiIndex& operator=(const multiIndex&) =default;
  multiIndex& operator=(multiIndex&&) =default;
  ~multiIndex() =default;

  void setAxis(const size_t,const size_t);

  bool operator==(const multiIndex&) const;
  bool operator>(const multiIndex&) const;
  bool operator<(const multiIndex&) const;

  size_t operator++();
  size_t operator++(const int);

  /// accessor to current value
  size_t get() const { return currentIndex; }
  
  void write(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&,const multiIndex&);


#endif
