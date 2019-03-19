/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/ShutterUnit.h
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
#ifndef xraySystem_ShutterUnit_h
#define xraySystem_ShutterUnit_h

class Simulation;

namespace xraySystem
{

/*!
  \class ShutterUnit
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focasable mirror in mount
*/

class ShutterUnit :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  bool upFlag;             ///< Up/down
  
  double height;           ///< height total 
  double width;            ///< width accross beam
  double thick;            ///< Thickness in normal direction to reflection  
  double baseLift;         ///< Amount to lift [when closed]
  double lift;             ///< Amount to lift [when raized]
  double liftScrewRadius;  ///< Radius of lifting thread
  double threadLength;     ///< Total length of thread
  
  double topInnerRadius;          ///<  Flange inner radius radius 
  double topFlangeRadius;          ///< Joining Flange radius 
  double topFlangeLength;          ///< Joining Flange length

  double bellowLength;             ///< Bellow length [compessed]
  double bellowThick;              ///< Bellow thick
  
  double outRadius;                ///< Out connect radius
  double outLength;                ///< Out connect length

  int blockMat;            ///<  Base material
  int flangeMat;           ///<  flange material
  int bellowMat;           ///<  bellow material
  int threadMat;            ///<  water material
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int,
			const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  std::vector<Geometry::Vec3D> calcEdgePoints() const;
  
 public:

  ShutterUnit(const std::string&);
  ShutterUnit(const ShutterUnit&);
  ShutterUnit& operator=(const ShutterUnit&);
  virtual ~ShutterUnit();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int);
  
};

}

#endif
 
