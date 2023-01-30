/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/HoleShape.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef constructSystem_HoleShape_h
#define constructSystem_HoleShape_h

class Simulation;

namespace constructSystem
{
  /*!
    \class HoleShape
    \version 1.0
    \author S. Ansell
    \date June 2015
    \brief Variable detemine hole type
  */
  
class HoleShape : 
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  size_t shapeType;             ///< Shape index

  double angleCentre;           ///< Mid point for alignment [for interigation]
  double angleOffset;           ///< Rotation around centre point
  double radialStep;            ///< Centre radial position
  
  double radius;                ///< Shape radius
  double xradius;               ///< Extra radius [if needed]

  Geometry::Vec3D rotCentre;       ///< Centre position
  double rotAngle;                 ///< Angle of whole system [true pos]

  size_t cutFlag;                    ///< Add faces to boundary cut [1/2]
  
  void setShape(const size_t);
  void setShape(const std::string&);

  void createCircleSurfaces();
  void createSquareSurfaces();  
  void createHexagonSurfaces();
  void createOctagonSurfaces();
  void createRectangleSurfaces();

  HeadRule createCircleObj() const;
  HeadRule createSquareObj() const;
  HeadRule createHexagonObj() const;
  HeadRule createOctagonObj() const;
  HeadRule createRectangleObj() const;


  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  HoleShape(const std::string&);
  HoleShape(const HoleShape&);
  HoleShape& operator=(const HoleShape&);
  virtual ~HoleShape() {}

  static size_t shapeIndex(const std::string&);

  void setCutFaceFlag(const size_t);
  /// accessor to shape
  size_t getShape() const { return shapeType; }

  void populate(const FuncDataBase&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
