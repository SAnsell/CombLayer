/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   workInc/Boundary.h
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
#ifndef Boundary_h
#define Boundary_h

/*!
  \struct BItems 
  \brief Holds components that are in boundary
  \version 1.0
  \author S. Ansell
  \date May 2008

  Each bin in the new item holds a BItems for
  each bin. That has a vector of the bins in the 
  original and the overlap fraction of the original
  within the new bin.
*/

struct BItems
{
 public:
  
  /// Item storage type
  typedef std::pair<size_t,double> PTYPE;
  /// Storage of index and fraction
  typedef std::vector<PTYPE> FTYPE;  
  /// Fraction list of items to include
  FTYPE FList;

  BItems();
  BItems(const BItems&);
  BItems& operator=(const BItems&);
  ~BItems() {}  ///< Destructor 

  /// Access to FList begin
  FTYPE::const_iterator begin() const { return FList.begin(); }
  /// Access to FList end
  FTYPE::const_iterator end() const { return FList.end(); }

  /// determine if empty
  bool isEmpty() const { return FList.empty(); }
  void addItem(const size_t,const double);  

  std::pair<size_t,double> getItem(const size_t) const;
  void write(std::ostream&) const;

};

/*!
  \class Boundary
  \version 1.0
  \author S. Ansell
  \date May 2008
  \brief Group of boundary componentds

  Each boundary is a group has a group of fractions that 
  correspontd to the indexes of Y values.
 */

class Boundary
{
 private:

  /// Master list of boundaries (New Regions components)
  std::vector<BItems> Components;
  size_t nonEmpty;                     ///< First component (with data)
  size_t backEmpty;                    ///< Last component+1 (with data)

  double getFrac(const double,const double,
		 const double,const double) const;
  void setEmpty();

 public:

  /// Storage of contributions to a boundary
  typedef std::vector<BItems> BTYPE;

  Boundary();
  Boundary(const Boundary&);
  Boundary& operator=(const Boundary&);
  ~Boundary() {};          ///< Destructor
  
  void setBoundary(const std::vector<double>&,const std::vector<double>&);
  void setBoundary(const std::vector<BUnit>&,const std::vector<BUnit>&);
  /// Begin Iterator to BItems 
  BTYPE::const_iterator begin() const 
    { return Components.begin()+static_cast<long int>(nonEmpty); }
  /// End Iterator to BItems 
  BTYPE::const_iterator end() const { return Components.begin()
      +static_cast<long int>(backEmpty); } 

  BItems::PTYPE getItem(const size_t,const size_t) const;
  /// Accessor to start point
  size_t getIndex() const { return nonEmpty; }
};

std::ostream&
operator<<(std::ostream&,const BItems&);

#endif



