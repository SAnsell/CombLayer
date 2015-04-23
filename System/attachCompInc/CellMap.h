/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/CellMap.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef attachSystem_CellMap_h
#define attachSystem_CellMap_h

namespace attachSystem
{
/*!
  \class CellMap
  \version 1.0
  \author S. Ansell
  \date March 2015
  \brief Cell mapping by name [experimental]
*/

class CellMap  
{
 private:

  typedef std::map<std::string,int> LCTYPE;
  
  LCTYPE Cells;   ///< Named cells
    
 public:

  CellMap();         
  CellMap(const CellMap&);
  CellMap& operator=(const CellMap&);
  virtual ~CellMap() {}     ///< Destructor
  
  void setCell(const std::string&,const int);
  void setCell(const std::string&,const size_t,const int);
  int getCell(const std::string&) const;
  int getCell(const std::string&,const size_t) const;
  
};

}

#endif
 
