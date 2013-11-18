/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystalInc/loopItem.h
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
#ifndef Crystal_loopItem_h
#define Crystal_loopItem_h

namespace Crystal
{
  /*!
    \class loopItem
    \version 1.0
    \author S. Ansell
    \date July 2009
    \brief Holds loop items for addition etc.
  */

class loopItem
{
 private:
  
  size_t cIndex;                      ///< Current index
  std::vector<std::string> Items;  ///< List of items [empty if so]

 public:
  
  explicit loopItem(const size_t);
  loopItem(const loopItem&);
  loopItem& operator=(const loopItem&);
  ~loopItem() {}   ///< Destructor
  
  void addItem(const size_t,const std::string&);
  int addNext(const std::string&);
  bool isEmpty() { return !cIndex; } ///< Is it empty
  /// Determine if full
  bool isFull() { return cIndex==Items.size(); } 
 
  const std::string& getItemStr(const size_t) const;

  template<typename T>
  int getItem(const size_t,T&) const;
  
  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const loopItem&);

}

#endif
