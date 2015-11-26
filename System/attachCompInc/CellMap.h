/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
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

class Simulation;

namespace attachSystem
{

  class ContainedComp;

/*!
  \class CellMap
  \version 1.0
  \author S. Ansell
  \date March 2015
  \brief Cell mapping by name [experimental]
*/

class CellMap  : public BaseMap
{
 private:
  
 public:

  CellMap();         
  CellMap(const CellMap&);
  CellMap& operator=(const CellMap&);
  virtual ~CellMap() {}     ///< Destructor

  /// Renmae function
  void setCell(const std::string& K,const int CN)
    { BaseMap::setItem(K,CN); }
      
  /// Renmae function
  void setCell(const std::string& K,const size_t Index,const int CN)
    { BaseMap::setItem(K,Index,CN); }
  
  void setCells(const std::string& K,const int CNA,const int CNB)
   { BaseMap::setItems(K,CNA,CNB); }

  /// Rename function
  void addCell(const std::string& K,const int CN)
    { BaseMap::addItem(K,CN); }
  /// Rename function
  void addCells(const std::string& K,const std::vector<int>& CN)
    { BaseMap::addItems(K,CN); }

  /// Rename function
  int getCell(const std::string& K) const
    { return BaseMap::getItem(K); }
  /// Rename function
  int getCell(const std::string& K,const size_t Index) const
    { return BaseMap::getItem(K,Index); }

  std::vector<int> getCells(const std::string& K) const
    { return BaseMap::getItems(K); }

  std::vector<int> getCells() const
    { return BaseMap::getItems(); }
    
  int removeCell(const std::string& K,const size_t Index=0)
    {  return BaseMap::removeItem(K,Index); }


  void insertComponent(Simulation&,const std::string&,
		       const ContainedComp&) const;
  void insertComponent(Simulation&,const std::string&,
		       const HeadRule&) const;
  void insertComponent(Simulation&,const std::string&,
		       const std::string&) const;
  void insertComponent(Simulation&,const std::string&,
		       const FixedComp&,const long int) const;

    
 void deleteCell(Simulation&,const std::string&,const size_t =0);
  std::pair<int,double>
    deleteCellWithData(Simulation&,const std::string&,const size_t =0);

};

}

#endif
 
