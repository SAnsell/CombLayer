/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystalInc/CifItem.h
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
#ifndef Crystal_CifItem_h
#define Crystal_CifItem_h

namespace Crystal
{
  
  /*!
    \class CifItem
    \author S. Ansell
    \version 1.0
    \date July 2009
    \brief Holds a loop component of a cif- file
  */

class CifItem
{
 private:

  int status;              ///< 0 empty / 1 KeyOnly / 2 Full

  std::string Key;    ///< Keys to data found
  std::string Data;       ///< Keys to data found

 public:

  CifItem();
  CifItem(const CifItem&);
  CifItem& operator=(const CifItem&);
  ~CifItem() {}                         ///< Destructor
  
  /// Access status
  int getStatus() const { return status; }
  /// Clear the system
  void clearAll();

  bool hasKey(const std::string&) const;
  int registerLine(std::string&);

  const std::string& getKey () const { return Key; } ///< Key accessor
  /// Access data
  const std::string& getData() const { return Data; }

  template<typename T>
  int getItem(T&);             // Clears status on success
 
};

}

#endif
