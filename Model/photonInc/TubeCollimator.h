/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/TubeCollimator.h
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
#ifndef photonSystem_TubeCollimator_h
#define photonSystem_TubeCollimator_h

class Simulation;

namespace constructSystem
{
  class hexUnit;
}

namespace photonSystem
{
/*!
  \class TubeCollimator
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief Mesh of rod inserted into an object
*/

class TubeCollimator : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:
  
  const int rodIndex;          ///< Index of surface offset
  int cellIndex;               ///< Cell index


  std::string layoutType;      ///< layout type
  std::string boundaryType;    ///< boundary type
  long int nTubeLayers;       ///< Number of tubes [-ve for boundary]
  
  double length;            ///< length
  double radius;            ///< Hole radius
  double wallThick;         ///< Hole liner radius [if any]
  double centSpc;           ///< Hole-Hole centre spacing
  int wallMat;              ///< Material liner
  
  double boundaryRotAngle;  ///< Rotation angle of the boundary
  double layoutRotAngle;    ///< Rotation angle of the layout axis
  
  Geometry::Vec3D AAxis;    ///< Basis set for layout [first]
  Geometry::Vec3D BAxis;    ///< Basic sef for layout [second]
  HeadRule boundary;        ///< Semi-infinite headRule for centre-boundary

  void setBoundary(const FuncDataBase&);
  void setLayout();
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createLinks();
  void createTubes(Simulation&);

  std::string boundaryString() const;
  
 public:

  TubeCollimator(const std::string&);
  TubeCollimator(const TubeCollimator&);
  TubeCollimator& operator=(const TubeCollimator&);
  ~TubeCollimator();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
