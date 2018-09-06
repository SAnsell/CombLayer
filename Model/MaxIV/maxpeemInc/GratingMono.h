/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/GratingMono.h
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
#ifndef xraySystem_GratingMono_h
#define xraySystem_GratingMono_h

class Simulation;

namespace xraySystem
{

/*!
  \class GratingMono
  \author S. Ansell
  \version 1.0
  \date September 2018
  \brief Paired Mono-crystal mirror constant exit gap
*/

class GratingMono :
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  Geometry::Vec3D rotCent;  ///< Rotation centre
  double theta;             ///< Theta angle

  Geometry::Vec3D mOffset;  ///< mirror offset [at zero theta]
  double mWidth;            ///< Width of lower mirror
  double mThick;            ///< 
  double mLength;           ///< Outer wall thickness

  Geometry::Vec3D gOffset;  ///< grateing offset [at zero theta]
  double gWidth;            ///< Radius of from centre
  double gThick;            ///< Radius of detector
  double gLength;           ///< Outer wall thickness

  double mBaseThick;        ///< mirror Base thickness
  double mBaseWidth;        ///< mirror width
  double mBaseLength;       ///< mirror length

  double gBaseThick;        ///< Base thickness
  double gBaseWidth;        ///< width of A bas
  double gBaseLength;       ///< Extra width/length of base

  int xtalMat;             ///< XStal material
  int baseMat;             ///< Base material

  Geometry::Vec3D MCentre;      ///< Mirror centre
  Geometry::Vec3D GCentre;      ///< Grating centre
  
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  GratingMono(const std::string&);
  GratingMono(const GratingMono&);
  GratingMono& operator=(const GratingMono&);
  virtual ~GratingMono();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
