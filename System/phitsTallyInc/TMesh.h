/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/TMesh.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
#ifndef phitsSystem_TMesh_h
#define phitsSystem_TMesh_h

namespace phitsSystem
{
/*!
  \class TMesh
  \version 1.0
  \date October 2020
  \author S. Ansell
  \brief 3d mesh tally for phits based on t-track
*/

class TMesh : public phitsTally
{
 private:


  MeshXYZ gridXYZ;                    ///< xyz mesh

  eType energy;                   ///< Energy grid / value

  std::string axis;               ///< Axis (eng/angle/xy/yz etc)
  int unit;                       ///< Unit 
    
 public:

  TMesh(const std::string&,const int);
  TMesh(const TMesh&);
  TMesh* clone() const override;  
  TMesh& operator=(const TMesh&);
  ~TMesh() override;

  /// set units
  void setUnit(const int A) { unit=A; }
  void setUnit(const std::string&);

  void setEnergy(const eType& A) override { energy = A; }

  void setIndex(const std::array<size_t,3>& IPts)
  {  gridXYZ.setIndex(IPts); }
  void setCoordinates(const Geometry::Vec3D& APt,
		      const Geometry::Vec3D& BPt)
  { gridXYZ.setCoordinates(APt,BPt); }
  void write(std::ostream&,const std::string&) const override;
  
};

}

#endif
