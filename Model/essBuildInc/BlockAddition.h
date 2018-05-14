/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BlockAddition.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
  \brief Addition to ESS block addiotn
*/

class BlockAddition :  public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int blockIndex;          ///< Index of surface offset
  int cellIndex;                 ///< Cell index
  int active;                    ///< Box active

  double length;                 ///< Length
  double height;                 ///< height
  double width;                  ///< width
  size_t nLayers;                ///< Number of layers
  std::vector<double> wallThick; ///< Wall Thick
  std::vector<double> wallTemp;  ///< Wall Temp
  std::vector<int> wallMat;      ///< Wall Mat

  int waterMat;                  ///< Water material

  int edgeSurf;                  ///< Surface to join too
  Geometry::Vec3D rotCent;       ///< General rotation centre
  std::string preModInner;       ///< Inner preMod surface
  std::string preModOuter;       ///< Outer preMod surface

  void populate(const FuncDataBase&);
  void createUnitVector(const Geometry::Vec3D&,
			const Geometry::Vec3D&,
			const Geometry::Vec3D&);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::LayerComp&,
		     const size_t,const long int);
  void createLinks();

  std::string rotateItem(std::string);

 public:

  BlockAddition(const std::string&);
  BlockAddition(const BlockAddition&);
  BlockAddition& operator=(const BlockAddition&);
  virtual ~BlockAddition();


  /// Set/unset active
  void setActive(const int A) { active=A; }
  /// Set edge surf
  void setEdgeSurf(const int S) { edgeSurf=S; }
  /// Set centre rotation
  void setCentRotate(const Geometry::Vec3D& C) { rotCent=C; }
  std::string createCut(const size_t) const;
  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::LayerComp&,
		 const size_t,const long int);

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;

};

}

#endif
 
