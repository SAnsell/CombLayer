/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/activeFluxPt.h
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
#ifndef SDef_activeFluxPt_h
#define SDef_activeFluxPt_h

namespace SDef
{

/*!
  \class activeFluxPt
  \version 1.0
  \author S. Ansell
  \date Novebmer 2016
  \brief Inverse of activeFluxPt - point /cell
*/

class activeFluxPt
{
 private:
  
  int cellN;                ///< Cell number
  Geometry::Vec3D fluxPt;   ///< position
  
 public:
  
  activeFluxPt(const int,const Geometry::Vec3D&);
  activeFluxPt(const activeFluxPt&);
  activeFluxPt& operator=(const activeFluxPt&);
  ~activeFluxPt();

  /// const accessor
  int getCellID() const { return cellN; }
  /// const accessor
  const Geometry::Vec3D& getPoint() const { return fluxPt; }
  
};

}

#endif
 
