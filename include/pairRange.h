/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/pairRange.h
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
#ifndef pairRange_h
#define pairRange_h

/*!
  \class pairRange
  \brief Ranges of intervals
  \author S. Ansell
  \date March 2011
  \version 1.0

  Modified to hold groups of values in an NRange type manor
*/

class pairRange
{
 private:

  /// Storage type
  typedef std::vector<std::pair<double,double> > PTYPE;
  
  PTYPE Items;     ///< A Values

 public:

  pairRange();
  pairRange(const std::string&);         ///< Initialise with a string
  pairRange(const pairRange&);  
  pairRange& operator=(const pairRange&);
  bool operator==(const pairRange&) const;
  ~pairRange();

  const std::pair<double,double>& operator[](const size_t) const;
  pairRange& operator+=(const pairRange&);

  bool empty() const { return Items.empty(); }  ///< Range is stored
  size_t size() const { return Items.size(); }    ///< Size 
  void clear() { Items.clear(); }                  ///< Clear range
  size_t count() const;

  int processString(const std::string&);       ///< process string
  void condense(const double = 1e-6);     
  void addComp(const double,const double);
  void setVector(const std::vector<double>&,
		 const std::vector<double>&);

  size_t writeVector(std::vector<std::pair<double,double> >&) const;
  void write(std::ostream&) const;             ///< Write out the range 

};

std::ostream&
operator<<(std::ostream&,const pairRange&);

#endif
