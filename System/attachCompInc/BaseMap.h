/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/BaseMap.h
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
#ifndef attachSystem_BaseMap_h
#define attachSystem_BaseMap_h

class Simulation;

namespace attachSystem
{
  class FixedComp;

/*!
  \class BaseMap
  \version 1.0
  \author S. Ansell
  \date March 2015
  \brief Cell mapping by name [experimental]
*/

class BaseMap  
{
 protected:

  /// Name to lists
  typedef std::map<std::string,int> LCTYPE;
  ///  Lists
  typedef std::vector<std::vector<int>> SEQTYPE;

  
  LCTYPE Items;          ///< Named cells
  SEQTYPE SplitUnits;    ///< Split named cells
  
 public:

  BaseMap();         
  BaseMap(const BaseMap&);
  BaseMap& operator=(const BaseMap&);
  virtual ~BaseMap() {}     ///< Destructor
  
  void setItem(const std::string&,const int);
  void setItem(const std::string&,const size_t,const int);
  void addItem(const std::string&,const int);
  int getItem(const std::string&) const;
  int getItem(const std::string&,const size_t) const;

  void setItems(const std::string&,const int,const int);

  void addItems(const std::string&,const std::vector<int>&);  

  std::vector<std::string> getNames() const;
  std::vector<int> getItems(const std::string&) const;
  std::vector<int> getItems() const;
  
  int removeItem(const std::string&,const size_t =0);
  
};

}

#endif
 
