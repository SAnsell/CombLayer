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

  /*!
    Types 
    - 0: null-op  [no write]
    - 1: j
    - 2: number [integer]
    - 4: number [double]
    - 8: repeat
    - 16: interval
    - 32: log
    - 64: NTree
  */
  std::vector<size_t> itemType;
  std::map<size_t,long int> numInt;           ///< num Inter
  std::map<size_t,double> numDbl;             ///< double number
  std::map<size_t,size_t> repeats;            ///< repeat units
  std::map<size_t,NTree> subTree;             ///< sub trees

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
  
  
  void addComp(const std::vector<double>&);
  void addComp(const std::vector<int>&);
  void addComp(const double&);
  void addComp(const int&);
  void addComp(const std::string&);


  void addUnits(const std::vector<Unit>&);

  void splitComp();
  int changeItem(const Unit&,const Unit&);
  
  int processString(const std::string&);  
  std::vector<double> actualValues() const;
  
  void write(std::ostream&) const;        

};

std::ostream&
operator<<(std::ostream&,const NTree&);

}   // NAMESPACE  tallySystem

#endif
