/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/BnId.h
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
#ifndef BnId_h
#define BnId_h 

/*!
  \class  BnId
  \brief Tri-state variable 
  \author S. Ansell
  \date April 2005
  \version 1.0

  
  This class holds a tri-state variable 
  of -1 (false) 0 (not-important) 1 (true) against
  each of the possible input desisions. It has
  arbiatary length (unlike using a long integer)
  \todo Could be improved by using a set of 
  unsigned integers. 
*/

class BnId
{
 private:

  static int fullOut;       ///< Full output for display

  size_t size;              ///< number of variables
  int PI;                   ///< Prime Implicant
  size_t Tnum;              ///< True number (1 in Tval)
  size_t Znum;              ///< Zero number (0 in Tval)
  std::vector<int> Tval;    ///< Truth values
  std::set<int> MinTerm;    ///< Minterms list

  void setCounters();    

 public:
  
  BnId();
  BnId(const size_t,const size_t );
  BnId(const BnId&);
  BnId& operator=(const BnId&);
  ~BnId();

  int operator==(const BnId&) const;  
  int operator<(const BnId&) const;   
  int operator>(const BnId&) const;   
  int operator[](const size_t) const;  
  int operator++(int);
  int operator++();   
  int operator--(int);
  int operator--();   

  /// Set a MinTerm
  void setMinTerm(const int);
  void addMinTerm(const BnId&); 
  int hasMinTerm(const int) const;

  int equivalent(const BnId&) const;    
  void reverse();     

  int PIstatus() const { return PI; }     ///< PI accessor
  void setPI(const int A) { PI=A; }       ///< PI accessor
  size_t intValue() const;              
  std::pair<int,BnId> 
    makeCombination(const BnId&) const;  

  /// Total requiring expression
  size_t expressCount() const { return size-Znum; } 
  /// returns number of variables / size          
  size_t Size() const { return size; }   
  /// Access true count
  size_t TrueCount() const { return Tnum; }
  
  void mapState(const std::vector<int>&,std::map<int,int>&) const;

  /// Set output state
  static void setFullDisplay(const int I) { fullOut=I; }
  std::string display() const;           
  void write(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&,const BnId&);

#endif
