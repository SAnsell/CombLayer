/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightInc/WWGWeight.h
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
#ifndef WeightSystem_WWGWeight_h
#define WeightSystem_WWGWeight_h

namespace WeightSystem
{

struct WWGItem
{
  size_t cX;              ///< Index
  double weight;          ///< weight for the cell
  double number;          ///< number of tracks

  /// Constructor
  WWGItem(const double W) : cX(0),weight(W), number(1.0) {}
  /// Copy construct
  WWGItem(const WWGItem& A) :
    cX(A.vWWG),weight(A.weight),number(A.number) {}
};
  
/*!
  \class WWGWeight
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Tracks cell weight in cells
*/
  
class WWGWeight
{
 private:
  
  const double sigmaScale;             ///< Scale for sigma
  double scaleFactor;                  ///< Scaling factor
  double minWeight;                    ///< Min weight
  std::map<int,WWGItem> Cells;        ///< Cells and track info

 public:

  WWGWeight();
  WWGWeight(const WWGWeight&);
  WWGWeight& operator=(const WWGWeight&);    
  ~WWGWeight() {}          ///< Destructor

  /// set the scale factor
  void setScaleFactor(const double F) { scaleFactor=F; }
  /// set min cell weight
  void setMinWeight(const double W) { minWeight=W; }
  void addTracks(const size_t,const double);
  void updateWM(const double) const;
  void write(std::ostream&) const;
};


std::ostream& operator<<(std::ostream&,const WWGWeight&);

}

#endif
