/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/TriangleMod.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef moderatorSystem_TriangleMod_h
#define moderatorSystem_TriangleMod_h

class Simulation;

namespace Geometry
{
  class Convex2D;
}

namespace moderatorSystem
{

/*!
  \class TriangleMod
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief TriangleMod [insert object]
*/

class TriangleMod : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:
  
  const int triIndex;      ///< Index of surface offset
  int cellIndex;            ///< Cell index

  double xStep;           ///< Offset relative to origin 
  double yStep;           ///< Offset relative to origin 
  double zStep;           ///< Offset relative to origin 

  double xyAngle;         ///< Offset relative to origin 
  double zAngle;          ///< Offset relative to origin 

  size_t nCorner;         ///< number of corner points
  
  Geometry::Vec3D centroid;          ///< Centre point for external/interal
  std::vector<Geometry::Vec3D> CornerPts;  ///< Points of trianlge/shape 
  std::vector<int> nonConvex;      ///< Points are non-convex

  size_t nInnerCorner;         ///< number of inner-corner points
  std::vector<Geometry::Vec3D> InnerPts;    ///< Points of inner shape

  double height;           ///< Height/depth
  double wallThick;        ///< Wall thickness
  double flatClearance;    ///< Tri-level clearance
  double topClearance;     ///< Top clearance
  double baseClearance;    ///< Base clearance

  double innerStep;        ///< Distance to inner triangle wall
  double innerWall;        ///< Inner wall thickness
  int innerMat;            ///< Inner material

  double modTemp;          ///< Moderator temperature

  int modMat;              ///< Moderator material
  int wallMat;             ///< Wall material
  int pCladMat;            ///< Al poison support
  int poisonMat;           ///< Poison (Gadolinium)

  Geometry::Vec3D corner(const size_t,const double) const;
  Geometry::Vec3D midNorm(const size_t) const;
  Geometry::Vec3D sideNorm(const std::pair<Geometry::Vec3D,
			   Geometry::Vec3D>&) const;
  std::pair<Geometry::Vec3D,Geometry::Vec3D>
    cornerPair(const std::vector<Geometry::Vec3D>&,
	       const size_t,const size_t,const double) const;
  std::string getOuterString() const;
  std::string getInnerString(const std::string&) const;

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createLinks();
  void createSurfaces();
  void createObjects(Simulation&);
  
  void createConvex();

 public:

  TriangleMod(const std::string&);
  TriangleMod(const TriangleMod&);
  TriangleMod& operator=(const TriangleMod&);
  /// Clone function
  virtual TriangleMod* clone() const { return new TriangleMod(*this); }
  virtual ~TriangleMod();

  // special for 5 sides!!
  virtual int getExitWindow(const size_t,std::vector<int>&) const;

  virtual void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
