/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   workInc/BUnit.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef BUnit_h
#define BUnit_h

namespace DError
{
class doublErr;
}
/*!
  \struct BUnit
  \version 1.0
  \author S. Ansell
  \date November 2010
  \brief Store a atomic storage unit
*/

struct BUnit
{
  double W;              ///< Weight
  double xA;             ///< Start of bin
  double xB;             ///< End of bin
  DError::doubleErr Y;   ///< Value in bin
  
  BUnit(const double&,const double&,const DError::doubleErr&);
  BUnit(const double&,const double&,
	const double&,const DError::doubleErr&);
  BUnit(const BUnit&); 
  BUnit& operator=(const BUnit&);
  ~BUnit() {}
  
  BUnit& operator+=(const BUnit&);
  BUnit& operator-=(const BUnit&);
  BUnit& operator*=(const BUnit&);
  BUnit& operator/=(const BUnit&);

  BUnit& operator+=(const DError::doubleErr&);
  BUnit& operator-=(const DError::doubleErr&);
  BUnit& operator*=(const DError::doubleErr&);
  BUnit& operator/=(const DError::doubleErr&);

  BUnit operator+(const DError::doubleErr&) const;
  BUnit operator-(const DError::doubleErr&) const;
  BUnit operator*(const DError::doubleErr&) const;
  BUnit operator/(const DError::doubleErr&) const;

  void norm();
  void write(std::ostream& OX) const;
};

std::ostream& operator<<(std::ostream&,const BUnit&);
  


#endif
