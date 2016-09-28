/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/activeUnit.h
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
#ifndef SDef_activeUnit_h
#define SDef_activeUnit_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class activeUnit
  \version 1.0
  \author S. Ansell
  \date September 2016
  \brief Creates an active projection source
*/

class activeUnit 
{
 private:
  
  double volume;            ///< Volume by MC calc
  double integralFlux;      ///< Total flux

  std::vector<double> energy;      ///< Integrated energy
  std::vector<double> cellFlux;    ///< Integrated flux [normalized to 1.0]

 public:
  
  activeUnit(const std::vector<double>&,const std::vector<double>&);
  activeUnit(const activeUnit&);
  activeUnit& operator=(const activeUnit&);
  ~activeUnit();

  double XInverse(const double) const;
  void normalize(const double);
  void writePhoton(std::ostream&,const Geometry::Vec3D&) const;

};

}

#endif
 
