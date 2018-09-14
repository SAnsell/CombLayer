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

  int axisDirection;                ///< Axis direction xy,yz,xz
  int meshType;                     ///< type / 10 / 0 for mesh
  
  int trcl;                         ///< transform number
  
  std::array<size_t,3> Pts;      ///< N-Points
  Geometry::Vec3D minCoord;      ///< Min coordinate
  Geometry::Vec3D maxCoord;      ///< Max coordinate
  
  void writeMesh(std::ostream&) const;
  
 public:

  explicit TGShow(const int);
  TGShow(const TGShow&);
  virtual TGShow* clone() const; 
  TGShow& operator=(const TGShow&);
  virtual ~TGShow();
  
  void setPointMesh() { meshType=0; }  ///< Set as a point mesh
  void setGridMesh() { meshType=10; }  ///< Set as a grid [default] mesh
  
  void setParticle(const std::string&);
  virtual void setDoseType(const std::string&,const std::string&);
 
  void setIndex(const std::array<size_t,3>&);
  void setCoordinates(const Geometry::Vec3D&,const Geometry::Vec3D&);

  /// Access the centre
  Geometry::Vec3D getCentre() const { return (minCoord+maxCoord)/2.0; }
  /// access min/max point
  const Geometry::Vec3D& getMinPt() const { return minCoord; }
  /// access min/max point
  const Geometry::Vec3D& getMaxPt() const { return maxCoord; }
  /// access min/max point
  const std::array<size_t,3>& getNPt() const { return Pts; }
  
  void writeCoordinates(std::ostream&) const;
  virtual void writeAuxScore(std::ostream&) const;
  virtual void write(std::ostream&) const;
  
};

}

#endif
