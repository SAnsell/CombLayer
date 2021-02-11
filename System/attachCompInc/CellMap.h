/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/CellMap.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
namespace MonteCarlo
{
  class Object;
}

namespace attachSystem
{

  class ContainedComp;
  class ContainedGroup;

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

  //@{
  /*!
    Rename transform functions to BaseMap
    \param K :: Key name 
    \param CN :: Offset index
  */

  /// Accessor to has Item (to avoid ambiguity with SurfMap)
  bool hasCell(const std::string& K,const size_t index =0) const
    { return BaseMap::hasItem(K,index); }
    
  /// Create named item
  void setCell(const std::string& K,const int CN)
    { BaseMap::setItem(K,CN); }
      
  /// Add specific named item
  void setCell(const std::string& K,const size_t Index,const int CN)
    { BaseMap::setItem(K,Index,CN); }

  /// Add named item range
  void setCells(const std::string& K,const int CNA,const int CNB)
   { BaseMap::setItems(K,CNA,CNB); }

  void setCells(const std::string& K,const std::vector<int>& CVec)
    { BaseMap::setItems(K,CVec); }

  /// Rename function
  void addCell(const std::string& K,const int CN)
    { BaseMap::addItem(K,CN); }
  /// Rename function
  void addCells(const std::string& K,const std::vector<int>& CVec)
    { BaseMap::addItems(K,CVec); }

  /// Rename function
  int getCell(const std::string& K) const
    { return BaseMap::getItem(K); }
  
  /// Rename function
  int getCell(const std::string& K,const size_t Index) const
    { return BaseMap::getItem(K,Index); }

  /// Rename function
  int getLastCell(const std::string& K) const
    { return BaseMap::getLastItem(K); }

  /// return all cells found
  size_t getNCells(const std::string& K) const
    { return BaseMap::getNItems(K); }

  /// return all cells found
  std::vector<int> getCells(const std::string& K) const
    { return BaseMap::getItems(K); }

  /// get ALL cells
  std::vector<int> getCells() const
    { return BaseMap::getItems(); }

  //@}
  
  HeadRule getCellsHR(const Simulation&,const std::string&) const;
  const HeadRule& getCellHR(const Simulation&,const std::string&,
		     const size_t =0) const;

  int getCellMat(const Simulation&,const std::string&,
		     const size_t =0) const;

  /// remmove a cell number [index is offset]
  std::string removeCellNumber(const int CN,const size_t Index =0)
    { return BaseMap::removeItemNumber(CN,Index); }
      
  /// remove a cell by name
  int removeCell(const std::string& K,const size_t Index=0)
    {  return BaseMap::removeItem(K,Index); }

  void renumberCell(const int,const int);

  // Insert the cellMap object into the cell
  void insertCellMapInCell(Simulation&,const std::string&,
			   const int) const;
  // Insert the cellMap object into the cell
  void insertCellMapInCell(Simulation&,const std::string&,
			   const size_t,const int) const;

  // These all insert the object into the cell map:
  void insertComponent(Simulation&,const std::string&,
		       const CellMap&,const std::string&) const;
  void insertComponent(Simulation&,const std::string&,const size_t,
		       const CellMap&,const std::string&,const size_t) const;
  void insertComponent(Simulation&,const std::string&,
		       const ContainedComp&) const;
  void insertComponent(Simulation&,const std::string&,
		       const ContainedGroup&) const;
  void insertComponent(Simulation&,const std::string&,const size_t,
		       const ContainedComp&) const;
  void insertComponent(Simulation&,const std::string&,
		       const HeadRule&) const;
  void insertComponent(Simulation&,const std::string&,const size_t,
		       const HeadRule&) const;
  void insertComponent(Simulation&,const std::string&,
		       const std::string&) const;
  void insertComponent(Simulation&,const std::string&,const size_t,
		       const std::string&) const;
  void insertComponent(Simulation&,const std::string&,const size_t,
		       const FixedComp&,const long int) const;
  void insertComponent(Simulation&,const std::string&,
		       const FixedComp&,const long int) const;


  MonteCarlo::Object*
  getCellObject(Simulation&,const std::string&,
		const size_t =0) const;

  
  void makeCell(const std::string&,
		Simulation&,const int,const int,const double,
		const std::string&);
  void makeCell(const std::string&,
		Simulation&,const int,const int,const double,
		const HeadRule&);
  
  void deleteCell(Simulation&,const std::string&,const size_t =0);

  std::pair<int,double>
    deleteCellWithData(Simulation&,const std::string&,const size_t =0);
  
};

}

#endif
 
