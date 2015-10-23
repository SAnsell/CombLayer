/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightInc/CellWeight.h
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
#ifndef WeightSystem_CellWeight_h
#define WeightSystem_CellWeight_h

namespace WeightSystem
{

struct CellItem
{
  double weight;
  double number;

  /// Constructor
  CellItem(const double W) : weight(W), number(1.0) {}
  CellItem(const CellItem& A) :
  weight(A.weight),number(A.number) {}
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
  
  const double sigmaScale;
  std::map<int,CellItem> Cells;

 public:

  CellWeight();
  CellWeight(const CellWeight&);
  CellWeight& operator=(const CellWeight&);    
  ~CellWeight() {}          ///< Destructor

  void addTracks(const int,const double);
  void updateWM() const;
  void write(std::ostream&) const;
};


std::ostream& operator<<(std::ostream&,const CellWeight&);

}

#endif
