/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightInc/CellWeight.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef WeightSystem_CellWeight_h
#define WeightSystem_CellWeight_h

namespace WeightSystem
{

/*!
  \struct CellItem
  \brief Singel cell item
  \author S. Ansell
  \date February 2012
*/
  
struct CellItem
{
  long int vCell;         ///< Void cell
  double weight;          ///< weight for the cell
  double number;          ///< number of tracks

  /// Constructor
  CellItem(const double W) : vCell(0),weight(W), number(1.0) {}
  /// Copy construct
  CellItem(const CellItem& A) :
    vCell(A.vCell),weight(A.weight),number(A.number) {}
};
  
/*!
  \class CellWeight
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Tracks cell weight in cells
*/
  
class CellWeight 
{
 private:

  /// storage for cells
  typedef std::map<long int,CellItem> CMapTYPE;

  const double sigmaScale;             ///< Scale for sigma 
  CMapTYPE Cells;                      ///< Cells and track info

  double calcMinWeight(const double,const double) const;
  double calcMaxWeight(const double,const double) const;
  
 public:

  CellWeight();
  CellWeight(const CellWeight&);
  CellWeight& operator=(const CellWeight&);    
  virtual ~CellWeight() {}          ///< Destructor

  void clear();
  void addTracks(const long int,const double);

  
  void updateWM(const double,const double,
		const double,const double) const;
  void invertWM(const double,const double,
		const double,const double) const;
  void write(std::ostream&) const;
  
};

std::ostream& operator<<(std::ostream&,const CellWeight&);
 
}

#endif
