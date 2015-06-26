/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/HoleUnit.h
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
#ifndef hutchSystem_HoleUnit_h
#define hutchSystem_HoleUnit_h

class Simulation;

namespace hutchSystem
{
  /*!
    \class HoleUnit
    \version 1.0
    \author S. Ansell
    \date April 2011
    Creates a hole
  */
  
class HoleUnit : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  ModelSupport::surfRegister& HMap;   ///< Surface register
  const int holeIndex;                ///< Hole index
  
  int shapeType;                ///< Shape index
  double AngleOffset;           ///< Angle round the hole
  double radialOffset;          ///< Centre radial position
  double radius;                ///< Shape radius
  double depth;                 ///< Depth of collimator

  int frontFace;                ///< Front face
  int backFace;                 ///< Back face

  Geometry::Vec3D Centre;       ///< Centre position

  void setShape(const int);
  void createUnitVector(const double,const attachSystem::FixedComp&);
  void createUnitVector(const double,const attachSystem::SecondTrack&);
  void createSurfaces();

  void createCircleSurfaces();
  void createSquareSurfaces();  
  void createHexagonSurfaces();
  void createOctagonSurfaces();

  std::string createCircleObj();
  std::string createSquareObj();
  std::string createHexagonObj();
  std::string createOctagonObj();

 public:
  
  HoleUnit(ModelSupport::surfRegister&,const std::string&,
	   const int);
  HoleUnit(const HoleUnit&);
  HoleUnit& operator=(const HoleUnit&);
  virtual ~HoleUnit() {}

  std::string createObjects();
  
  void populate(const FuncDataBase&);
  void setFaces(const int,const int);

  void createAll(const double,const attachSystem::FixedComp&);
  
  /// Access the angle
  double getAngle() const { return AngleOffset; }
  /// Get Centre point [relative to rotation point]
  const Geometry::Vec3D& getCentre() const  { return Centre; }

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;
};

}

#endif
 
