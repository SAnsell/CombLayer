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
  class gridUnit;
  class hexUnit;
}

namespace photonSystem
{
/*!
  \class TubeCollimator
  \version 1.0
  \author S. Ansell
  \date October 2016
  \brief Mesh of rod inserted into an object
*/

class TubeCollimator : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  typedef std::map<int,constructSystem::gridUnit*> MTYPE;
  
  const int rodIndex;          ///< Index of surface offset
  int cellIndex;               ///< Cell index


  std::string layoutType;      ///< layout type
  std::string boundaryType;    ///< boundary type
  size_t nLinks;               ///< Hole links
  long int nTubeLayers;       ///< Number of tubes [-ve for boundary]

  MTYPE GGrid;    ///< Rec/Hex vector
  
  double length;            ///< length
  double radius;            ///< Hole radius
  double wallThick;         ///< Hole liner radius [if any]
  double centSpc;           ///< Hole-Hole centre spacing
  int wallMat;              ///< Material liner
  
  double boundaryRotAngle;  ///< Rotation angle of the boundary
  double layoutRotAngle;    ///< Rotation angle of the layout axis
  bool gridExclude;         ///< Exclude the grid
  
  Geometry::Vec3D AAxis;    ///< Basis set for layout [first]
  Geometry::Vec3D BAxis;    ///< Basic sef for layout [second]
  Geometry::Vec3D CAxis;    ///< Basic sef for layout [third] 

  HeadRule boundary;        ///< Semi-infinite headRule for centre-boundary
  HeadRule voidBoundary;    ///< Semi-infinite headRule for actual void

  void clearGGrid();
  constructSystem::gridUnit* newGridUnit
    (const long int,const long int,const Geometry::Vec3D&) const;
  const Geometry::Vec3D& getGridAxis(const size_t) const;
  
  void setBoundary(const FuncDataBase&);
  void setLayout();
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createLinks();

  void createCentres();
  void createJoinLinks();
  void createJoinSurf();  
  void createCells(Simulation&);
  std::string calcBoundary(const constructSystem::gridUnit*) const;
  void createTubes(Simulation&);

  std::string boundaryString() const;
  bool calcGapInside(constructSystem::gridUnit*);

  
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
 
