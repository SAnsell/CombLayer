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

class CellMap  
{
 private:

  typedef std::map<std::string,int> LCTYPE;
  typedef std::vector<std::vector<int>> SEQTYPE;

  
  LCTYPE Cells;   ///< Named cells
  SEQTYPE SplitUnits;    ///< Split named cells
  
 public:

  CellMap();         
  CellMap(const CellMap&);
  CellMap& operator=(const CellMap&);
  virtual ~CellMap() {}     ///< Destructor
  
  void setCell(const std::string&,const int);
  void setCell(const std::string&,const size_t,const int);
  void addCell(const std::string&,const int);
  int getCell(const std::string&) const;
  int getCell(const std::string&,const size_t) const;

  void setCells(const std::string&,const int,const int);  
  std::vector<int> getCells(const std::string&) const;

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
  
  int removeCell(const std::string&,const size_t =0);
  
};

}

#endif
 
