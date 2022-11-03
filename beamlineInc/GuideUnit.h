/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamlineInc/GuideUnit.h
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
#ifndef beamlineSystem_GuideUnit_h
#define beamlineSystem_GuideUnit_h

class Simulation;


namespace beamlineSystem
{

class ShapeUnit;

/*!
  \class GuideUnit
  \version 1.0
  \author S. Ansell
  \date April 2014
  \brief Basic beamline guide unit
*/

class GuideUnit :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 protected:

  bool resetYRotation;        ///< Reset rotoation in y used for bend axis
  
  Geometry::Vec3D begPt;   ///< Current start point
  Geometry::Vec3D endPt;   ///< Current exit point

  // OUTER DIMENTIONS:
  double length;                     ///< Full length

  size_t nShapeLayers;              ///< Number of shapeLayers
  std::vector<double> layerThick;   ///< Thickness [inner->outer]
  std::vector<int> layerMat;        ///< Mat

  virtual void populate(const FuncDataBase&);
  virtual void createSurfaces() =0;
  virtual void createObjects(Simulation&) =0;
  virtual void createLinks();
  
 public:

  GuideUnit(const std::string&);
  GuideUnit(const GuideUnit&);
  GuideUnit& operator=(const GuideUnit&);
  virtual ~GuideUnit();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);
  
};

}

#endif
 
