/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLiterator.h
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
#ifndef XMLiterator_h
#define XMLiterator_h

namespace XML
{

/*!
  \class XMLiterator
  \author S. Ansell
  \version 1.0
  \date September 2007
  \brief Allows stepping through an XML schema
  
  Maintains a stack of the positions within
  a set of XMLgroups, so that a group can be
  completed and then stepped out of
  - BaseT :: Current base object
  - GroupT :: Top level group 
*/

class XMLiterator
{
 private:

  /// Storage for markPoint
  typedef std::pair<const XMLgroup*,int> MVAL;  
  /// Mark stack storage
  typedef std::vector<MVAL> MVEC;

  int markNum;                   ///< Next mark number 
  int count;                     ///< Number of components
  int hold;                      ///< Number to hold (on ++)
  const XMLgroup* GPtr;          ///< Current group pointer
  const XMLobject* CPtr;         ///< Pointer to the current object

  MVEC markGrp;                  ///< Marked group
  std::set<int> markSet;         ///< Active units
  const XMLgroup* Master;        ///< Pointer to the top object 

  std::vector<long int> Pos;              ///< Positions in the stack
  std::vector<const XMLgroup*> XG;   ///< List of groups


  void clearMark();


 public:
  
  XMLiterator(const XMLgroup*);
  XMLiterator(const XMLiterator&);
  XMLiterator& operator=(const XMLiterator&);
  ~XMLiterator() {}  ///< Destructor
  
  void init();

  /// post-fix increment
  int operator++(const int) { return operator++(); } 
  /// post-fix decrement
  int operator--(const int) { return operator--(); }

  int operator++();
  int operator--();

  /// Add a one stop hold
  void addHold() { hold++; }
  /// Get the number depth of items
  int getLevel() const;
  /// Access number of components read
  int getCount() const { return count; }
  /// Accessor to the base object
  const XMLobject* operator*() const { return CPtr; }
  /// Accessor to the base object
  const XMLobject* operator->() { return CPtr; }
  /// Is a group header
  bool isGroup() { return (GPtr==CPtr); }

  int skipGroup();
  int seek(const std::string&,const int =0);
  int seekInternal(const std::string&,const int =0);

  int setMark(); 
  int hasMark(const int) const; 
  void writePos() const;

};

}

#endif
  
