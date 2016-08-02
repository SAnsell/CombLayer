/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/SurfMap.h
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
#ifndef attachSystem_SurfMap_h
#define attachSystem_SurfMap_h

class Simulation;

namespace attachSystem
{
  class FixedComp;
  class ContainedComp;

/*!
  \class SurfMap
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Surf mapping by name [experimental]
*/

class SurfMap : public BaseMap
{

 public:

  SurfMap();         
  SurfMap(const SurfMap&);
  SurfMap& operator=(const SurfMap&);
  virtual ~SurfMap() {}     ///< Destructor

  /// Renaming function
  void setSurf(const std::string& K,const int CN)
    { BaseMap::setItem(K,CN); }
      
  /// Renaming function
  void setSurf(const std::string& K,const size_t Index,const int CN)
    { BaseMap::setItem(K,Index,CN); }

  /// Renaming function 
  void setSurfs(const std::string& K,const int CNA,const int CNB)
    { BaseMap::setItems(K,CNA,CNB); }

  /// Renaming function
  void addSurf(const std::string& K,const int CN)
    { BaseMap::addItem(K,CN); }
  /// Renaming function
  void addSurfs(const std::string& K,const std::vector<int>& CN)
    { BaseMap::addItems(K,CN); }

  /// Renaming function
  int getSurf(const std::string& K) const
    { return BaseMap::getItem(K); }
  /// Renaming function
  int getSurf(const std::string& K,const size_t Index) const
    { return BaseMap::getItem(K,Index); }

  /// Renaming function
  std::vector<int> getSurfs(const std::string& K) const
    { return BaseMap::getItems(K); }
  ///  Renaming function
  std::vector<int> getSurfs() const
    { return BaseMap::getItems(); }
    
};

}

#endif
 
