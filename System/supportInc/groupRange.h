/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/groupRange.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef groupRange_h
#define groupRange_h

/*!
  \class Range
  \author S. Ansell
  \date September 2018
  \version 1.0
  \brief Manages integer ranged sets 
*/

class groupRange
{
 private:

  std::vector<int> LowUnit;      ///< units of low value [ordered]
  std::vector<int> HighUnit;     ///< Unit high value  [ordered]

  size_t valueIndex(const int) const;
  size_t validIndex(const int) const;
    
public:

  /// constructor
  groupRange();
  explicit groupRange(const int);
  explicit groupRange(const std::vector<int>&);
  groupRange(const int,const int);
  groupRange(const groupRange&);
  groupRange& operator=(const groupRange&);
  ~groupRange() {}    ///< Destructor

  bool operator==(const groupRange&) const;
  bool operator!=(const groupRange&) const;
  
  bool empty() const { return LowUnit.empty(); }     ///< empty flag
  void merge();  
  bool valid(const int) const;
  groupRange& combine(const groupRange&);

  void setItems(const std::vector<int>&);
  
  void addItem(const int);
  void addItem(const int,const int);
  void addItem(const std::vector<int>&);
  
  void removeItem(const int); 
  void move(const int,const int);
  std::vector<int> getAllCells() const;

  
  int getFirst() const;
  int getLast() const;
  int getNext(const int) const;
  
  void write(std::ostream&) const;
}; 

std::ostream&
operator<<(std::ostream&,const groupRange&);


#endif
