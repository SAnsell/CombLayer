/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/NTree.h
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
#ifndef NTree_h
#define NTree_h


/*!
  \class NTree
  \brief Ranges of integer/values  
  \author S. Ansell
  \date March 2016
  \version 1.0

  Holds a set of values. This bracket like nature
  is accommondate in a tree and the modifiers are 
  accommodated in the range sub-unit
*/

class NTree
{
 private:

  /// Type of item
  enum class IType : size_t 
   { 
     nullOp = 0,         // No write
       j = 1,            // skip,
       integer = 2,
       dble = 4 ,
       repeat = 8,
       interval = 16,
       log = 32,
       ntree = 64
    };

  std::vector<size_t> itemType;               ///< order item type
  std::map<size_t,long int> numInt;           ///< [pos] num Inter
  std::map<size_t,double> numDbl;             ///< [pos] double number
  std::map<size_t,size_t> repeats;            ///< [pos] repeat units
  std::map<size_t,NTree> subTree;             ///< [pos] sub trees

  void clearAll();
	     
 public:

  NTree();
  NTree(const std::string&);         ///< Initialise with a string
  NTree(const NTree&);  
  NTree& operator=(const NTree&);
  ~NTree();

  /// is Tree empty
  bool empty() const { return itemType.empty(); } 
  void clear();
  size_t count() const;  
  
  
  void addUnits(const std::vector<double>&);
  void addUnits(const std::vector<int>&);

  void addComp(const double&);
  void addComp(const int&);
  void addComp(const std::string&);


  int processString(const std::string&);  
  std::vector<double> actualValues() const;

  std::string str() const;
  void write(std::ostream&) const;        

};

std::ostream&
operator<<(std::ostream&,const NTree&);



#endif
