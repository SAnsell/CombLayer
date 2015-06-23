/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/HoleShape.h
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
#ifndef hutchSystem_HoleShape_h
#define hutchSystem_HoleShape_h

class Simulation;

namespace hutchSystem
{
  /*!
    \class HoleShape
    \version 1.0
    \author S. Ansell
    \date June 2015
    Creates a hole
  */
  
class HoleShape : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:

  const int holeIndex;          ///< Hole index
  
  int shapeType;                ///< Shape index

  double angleOffset;           ///< Rotation around centre point
  double radialOffset;          ///< Centre radial position
  
  double radius;                ///< Shape radius

  Geometry::Vec3D rotCentre;       ///< Centre position
  double rotAngle;                 ///< Angle of whole system

  HeadRule frontFace;                ///< Front face
  HeadRule backFace;                 ///< Back face

  
  void setShape(const size_t);

  void createCircleSurfaces();
  void createSquareSurfaces();  
  void createHexagonSurfaces();
  void createOctagonSurfaces();

  std::string createCircleObj();
  std::string createSquareObj();
  std::string createHexagonObj();
  std::string createOctagonObj();

  void populate(const FuncDataBase&);
  void createUnitVector(const double,const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  
 public:
  
  HoleShape(ModelSupport::surfRegister&,const std::string&,
	   const int);
  HoleShape(const HoleShape&);
  HoleShape& operator=(const HoleShape&);
  virtual ~HoleShape() {}

  std::string createObjects();
  
  void setFaces(const int,const int);
  void setFaces(const int,const int);
  
  void createAll(Simulation& System,
		 xconst attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
