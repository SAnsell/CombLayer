/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/NRange.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef Nrange_h
#define Nrange_h

/*!
  \class NRange
  \brief Ranges of intervals
  \author S. Ansell
  \date Sept 2005
  \version 1.1

  Modified to hold all ranges ie 1 10i 
  Holds a range between two value of the type 1.0 10log 10.0
  The NRunit type is described as
  - 0 x V :: Value no other
  - 1 N V :: repeat
  - 2 N V+1 :: interval [V is the next interval value]
  - 3 N V+1 :: log/interval 
*/

class NRange
{
 public:

  static int identVal(const double&,const double&,const double&); 
  static int intervalVal(const double&,const double&,
			 const double&,const double&); 
  static int logIntVal(const double&,const double&,
		       const double&,const double&); 

 private:

  typedef DTriple<int,int,double> NRunit; ///< Storage type in list
  
  std::list<NRunit> Items;                ///< List of number Items

 public:

  NRange();
  NRange(const std::string&);    
  NRange(const NRange&);  
  NRange& operator=(const NRange&);
  ~NRange();

  double operator[](const int) const;
  NRange& operator+=(const NRange&);

  int empty() const { return Items.empty(); }  ///< Range is stored
  size_t size() const { return Items.size(); }    ///< Size 
  void clear() { Items.clear(); }                  ///< Clear range
  int count() const;

  int processString(const std::string&);       ///< process string
  void condense(const double = 1e-6);     
  void addComp(const double);
  template<typename T>
  void setVector(const std::vector<T>&);
  size_t writeVector(std::vector<double>&) const;
  void write(std::ostream&) const;             ///< Write out the range 

};

std::ostream&
operator<<(std::ostream&,const NRange&);

#endif
