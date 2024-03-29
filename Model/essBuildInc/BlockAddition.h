/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BlockAddition.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef essSystem_BlockAddition_h
#define essSystem_BlockAddition_h

class Simulation;

namespace essSystem
{

/*!
  \class BlockAddition
  \author S. Ansell
  \version 1.0
  \date April 2013
  \brief Additional waterblock to ESS moderator
*/

class BlockAddition :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::ExternalCut
{
 private:
  
  int active;                    ///< Box active

  double length;                 ///< Length
  double height;                 ///< height
  double width;                  ///< width

  std::vector<double> wallThick; ///< Wall Thick
  std::vector<double> wallTemp;  ///< Wall Temp
  std::vector<int> wallMat;      ///< Wall Mat

  int waterMat;                  ///< Water material

  int edgeSurf;                  ///< Surface to join too
  Geometry::Vec3D rotCent;       ///< General rotation centre
  HeadRule preModInner;       ///< Inner preMod surface
  HeadRule preModOuter;       ///< Outer preMod surface

  void populate(const FuncDataBase&) override;
  void createUnitVector(const Geometry::Vec3D&,
			const Geometry::Vec3D&,
			const Geometry::Vec3D&) override;

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::LayerComp&,
		     const size_t,const long int);
  void createLinks();

  HeadRule rotateItem(HeadRule);

 public:

  BlockAddition(const std::string&);
  BlockAddition(const BlockAddition&);
  BlockAddition& operator=(const BlockAddition&);
  ~BlockAddition() override;


  /// Set/unset active
  void setActive(const int A) { active=A; }
  /// Set edge surf
  void setEdgeSurf(const int S) { edgeSurf=S; }
  /// Set centre rotation
  void setCentRotate(const Geometry::Vec3D& C) { rotCent=C; }
  HeadRule createCut(const size_t) const;

  Geometry::Vec3D getSurfacePoint(const size_t,const long int) const override;
  HeadRule getLayerHR(const size_t,const long int) const override;

  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::LayerComp&,
		 const size_t,const long int);

};

}

#endif
 
