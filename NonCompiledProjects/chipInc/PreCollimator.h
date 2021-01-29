/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/PreCollimator.h
*
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef hutchSystem_PreCollimator_h
#define hutchSystem_PreCollimator_h

class Simulation;

namespace hutchSystem
{

/*!
  \class PreCollimator
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief PreCollimator [insert object]
*/

class PreCollimator : public attachSystem::ContainedComp,
    public attachSystem::FixedOffsetGroup
{
 private:
  
  Geometry::Vec3D Centre;

  double radius;            ///< Full radius
  double depth;             ///< Full depth

  int defMat;               ///< Material for roof
  size_t holeIndex;         ///< Index of the hole 
  double holeAngOffset;     ///< Hole Angle offest
  
  double innerWall;          ///< Inner wall thickness
  int innerWallMat;          ///< Inner wall Material
  

  size_t nHole;           ///< number of holes
  size_t nLayers;         ///< number of layers
  std::vector<HoleUnit> Holes;  ///< Holes
  std::vector<double> cFrac;    ///< coll Layer thicknesss (fractions)
  std::vector<int> cMat;        ///< coll Layer materials

  std::vector<int> CDivideList;   ///< Cell divide List for 

  void setHoleIndex(const size_t,const double);

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

 public:

  PreCollimator(const std::string&);
  PreCollimator(const PreCollimator&);
  PreCollimator& operator=(const PreCollimator&);
  ~PreCollimator();

  Geometry::Vec3D getHoleCentre() const;
  void setCentre(const Geometry::Vec3D&);

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  void createPartial(Simulation&,const attachSystem::FixedComp&,
		     const long int);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
