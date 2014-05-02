/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/NList.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef tallySystem_NList_h
#define tallySystem_NList_h

namespace tallySystem
{
/*!
  \class NList
  \brief Ranges of integer/values  
  \author S. Ansell
  \date July 2006
  \version 1.0
  \tparam Unit :: data type (double/int)

  Holds a set of values. These values
  can be compbined with : (impling add) and can be
  interger / double + string. Each is written an ( value ) 
  if more than one exists for a line
*/

template<typename Unit>
class NList
{
 private:

  /*! 
   Storage type in list 
    - int == 1 :: the use string
    - int == 0  :: Unit has the value.
  */
  typedef DTriple<int,Unit,std::string> CompUnit;   
  
  std::vector<CompUnit> Items;                     ///< List of number Items

 public:

  NList();
  NList(const std::string&);         ///< Initialise with a string
  NList(const NList&);  
  NList& operator=(const NList&);
  ~NList();

  int empty() const { return Items.empty(); }  ///< Item exists
  size_t size() const { return Items.size(); }  ///< Size of list
  void clear() { Items.clear(); }            ///< Clear vector
  int count() const; 
  
  
  void addComp(const std::vector<Unit>&);
  void addComp(const Unit&);
  void addComp(const std::string&);
  
  void addUnits(const std::vector<Unit>&);

  int changeItem(const Unit&,const Unit&);
  
  int processString(const std::string&);    ///< process string
  std::vector<Unit> actualItems() const;           ///< Just the items
  void write(std::ostream&) const;                ///< Write out the range 

};

template<typename Unit>
std::ostream&
operator<<(std::ostream&,const NList<Unit>&);

}   // NAMESPACE  tallySystem

#endif
