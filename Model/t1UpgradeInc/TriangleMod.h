/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/TriangleMod.h
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
  \date January 2014
  \brief TriangleMod [insert object]
*/

class TriangleMod : public constructSystem::ModBase
{
 private:

  double absXStep;           ///< Offset relative to absolute
  double absYStep;           ///< Offset relative to absolute
  double absZStep;           ///< Offset relative to absolute
  double absXYAngle;         ///< Rotation xy plane
  double absZAngle;          ///< Rotation z plane 

  TriUnit Outer;          ///< Outer points 

  size_t nIUnits;               ///< number of inner units
  std::vector<TriUnit> IUnits;  ///< Inner unit data

  double height;           ///< Height/depth
  double wallThick;        ///< Wall thickness
  double flatClearance;    ///< Tri-level clearance
  double topClearance;     ///< Top clearance
  double baseClearance;    ///< Base clearance

  size_t nInnerLayers;             ///< Number of inner layers
  std::vector<double> innerThick;   ///< Inner wall thickness [out-to-in]
  std::vector<int> innerMat;       ///< Inner wall mat

  double modTemp;          ///< Moderator temperature

  int modMat;              ///< Moderator material
  int wallMat;             ///< Wall material

  Geometry::Vec3D realPt(const Geometry::Vec3D&) const;
  Geometry::Vec3D realAxis(const Geometry::Vec3D&) const;
  Geometry::Vec3D sideNorm(const std::pair<Geometry::Vec3D,
			   Geometry::Vec3D>&) const;
  std::pair<Geometry::Vec3D,Geometry::Vec3D>
    cornerPair(const std::vector<Geometry::Vec3D>&,
	       const size_t,const size_t,const double) const;

  std::string createInnerObject(Simulation&);
  void cutInnerObj(Simulation&,const TriUnit&);
  void cutOuterObj(Simulation&,const TriUnit&,const TriUnit&);
  
  virtual void populate(const FuncDataBase&);

  void createLinks();
  void createSurfaces();
  void createObjects(Simulation&);
  
  void createConvex();
  void createConvexUnit(std::vector<Geometry::Vec3D>&,
			std::vector<int>&) const;
  void calcOverlaps(Simulation& System);


 public:

  TriangleMod(const std::string&);
  TriangleMod(const TriangleMod&);
  TriangleMod& operator=(const TriangleMod&);
  /// Clone function
  virtual TriangleMod* clone() const { return new TriangleMod(*this); }
  virtual ~TriangleMod();

  // special for 5 sides!!
  virtual int getExitWindow(const long int,std::vector<int>&) const;

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;


  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const attachSystem::FixedComp*,const long int);
  
};

}

#endif
 
