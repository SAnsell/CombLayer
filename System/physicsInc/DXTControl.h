/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/DXTControl.h
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
#ifndef physicsSystem_DXTControl_h
#define physicsSystem_DXTControl_h

namespace physicsSystem
{
 
/*!
  \class DXTControl
  \version 2.0
  \date November 2015
  \author S.Ansell
  \brief Dxtran sphere controller
*/

class DXTControl 
{
 private:

  std::string particle;                    ///< Particle flag
  std::vector<int> rotateFlag;             ///< Items needing rotation
  std::vector<Geometry::Vec3D> Centres;    ///< Centres of dxtran 
  std::vector<double> RadiiInner;          ///< Radii
  std::vector<double> RadiiOuter;          ///< Radii

  std::vector<double> DDk;                 ///< K value for DD cutting
  std::vector<double> DDm;                 ///< M value for DD if k>0.0
  
 public:
   
  DXTControl();
  DXTControl(const DXTControl&);
  DXTControl& operator=(const DXTControl&);
  virtual ~DXTControl();

  void clear();
  void setUnit(const Geometry::Vec3D&,const double,const double,const bool);
  void setDD(const double,const double);

  void write(std::ostream&) const;
  
};

}

#endif
