/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/SurfMap.h
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
  \date March 2015
  \brief Surf mapping by name [experimental]
*/

class SurfMap  
{
 private:

  /// Name to lists
  typedef std::map<std::string,int> LCTYPE;
  ///  Lists
  typedef std::vector<std::vector<int>> SEQTYPE;

  
  LCTYPE Surfs;          ///< Named cells
  SEQTYPE SplitUnits;    ///< Split named cells
  
 public:

  SurfMap();         
  SurfMap(const SurfMap&);
  SurfMap& operator=(const SurfMap&);
  virtual ~SurfMap() {}     ///< Destructor

  /// Renmae function
  void setSurf(const std::string& K,const int CN)
    { BaseMap::setItem(K,CN); }
      
  /// Renmae function
  void setSurf(const std::string& K,const size_t Index,const int CN)
    { BaseMap::setItem(K,Index,CN); }

  void setSurfs(const std::string& K,const int CNA,const int CNB);
  { BaseMap::setItems(K,Index,CNA,CNB); }

  /// Rename function
  void addSurf(const std::string& K,const int CN)
    { BaseMap::addItem(K,CN); }

  /// Rename function
  int getSurf(const std::string& K) const
    { return BaseMap::getSurf(K); }
  /// Rename function
  int getSurf(const std::string& K,const size_t Index) const
    { return BaseMap::getSurf(K,Index); }

  std::vector<int> getSurfs(const std::string& K) const;
    { return BaseMap::getItems(K); }
    
  int removeSurf(const std::string&,const size_t Index=0);
    {  BaseMap::removeItem(K,Index); }


    
 void deleteSurf(Simulation&,const std::string&,const size_t =0);
  std::pair<int,double>
    deleteSurfWithData(Simulation&,const std::string&,const size_t =0);

};

}

#endif
 
