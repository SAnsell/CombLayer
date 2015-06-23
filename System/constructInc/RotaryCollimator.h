/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/RotaryCollimator.h
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
#ifndef constructSystem_RotaryCollimator_h
#define constructSystem_RotaryCollimator_h

class Simulation;

namespace constructSystem
{

/*!
  \class RotaryCollimator
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief RotaryCollimator [insert object]
*/

class RotaryCollimator : public attachSystem::ContainedComp,
    public attachSystem::FixedGroup
{
 private:
  
  const int colIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xyAngle;           ///< Angle relative to LC 
  double zAngle;            ///< Angle relative LC 
  double xStep;             ///< Offset on XAxis of centre
  double yStep;             ///< origin forward step 
  double zStep;             ///< Offset on ZAxis of centre

  double rotDepth;          ///< Distance to main rotation centre

  double radius;            ///< Full radius
  double thick;             ///< Thickness

  int defMat;               ///< Default Material 

  size_t holeIndex;         ///< Index of the hole 
  double holeAngOffset;     ///< Hole Angle offest
  
  double innerWall;          ///< Inner wall thickness
  int innerWallMat;          ///< Inner wall Material
  
  size_t nHole;           ///< number of holes
  size_t nLayers;         ///< number of layers
  std::vector<HoleShape> Holes;  ///< Holes

  std::vector<double> cFrac;    ///< coll Layer thicknesss (fractions)
  std::vector<int> cMat;        ///< coll Layer materials

  void setHoleIndex(const size_t,const double);

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

 public:

  RotaryCollimator(const std::string&);
  RotaryCollimator(const RotaryCollimator&);
  RotaryCollimator& operator=(const RotaryCollimator&);
  ~RotaryCollimator();

  Geometry::Vec3D getHoleCentre() const;
  void setCentre(const Geometry::Vec3D&);

  void createPartial(Simulation&,const attachSystem::FixedComp&);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
