/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/TGShow.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef phitsSystem_TGShow_h
#define phitsSystem_TGShow_h

namespace phitsSystem
{
/*!
  \class TGShow
  \version 1.0
  \date Ferbruary 2018
  \author S. Ansell
  \brief userbin for phits
*/

class TGShow : public phitsTally
{
 private:

  MeshXYZ mesh;                     ///< Mesh volume
  int axisDirection;                ///< Axis direction xy,yz,xz
  int trcl;                         ///< transform number
  
 public:

  explicit TGShow(const int);
  TGShow(const TGShow&);
  virtual TGShow* clone() const; 
  TGShow& operator=(const TGShow&);
  virtual ~TGShow();
     
  void setIndex(const std::array<size_t,3>&);
  void setCoordinates(const Geometry::Vec3D&,const Geometry::Vec3D&);
  
  virtual void write(std::ostream&) const;
  
};

}

#endif
