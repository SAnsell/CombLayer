/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/UTubePipe.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_UTubePipe_h
#define xraySystem_UTubePipe_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class UTubePipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief UTubePipe unit  
*/

class UTubePipe :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:
  
  bool frontJoin;               ///< Flag for front join to calc midpoint
  Geometry::Vec3D FPt;          ///< Front point
  Geometry::Vec3D FAxis;        ///< Front axis

  bool backJoin;                ///< Flag for back join to calc midpoint
  Geometry::Vec3D BPt;          ///< Back point for join
  Geometry::Vec3D BAxis;        ///< Back axis for join

  double width;                 ///< void width [inner]
  double height;                ///< void height [inner]
  double length;                ///< void length [total]
  
  double feThick;               ///< pipe thickness

  double flangeARadius;          ///< Joining Flange radius 
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius 
  double flangeBLength;          ///< Joining Flange length
    
  int voidMat;                  ///< Void material
  int feMat;                    ///< Pipe material

  Geometry::Vec3D flangeAYAxis;        ///< front axis for flange
  Geometry::Vec3D flangeBYAxis;        ///< Back axis for flange
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void applyActiveFrontBack();

  
 public:

  UTubePipe(const std::string&);
  UTubePipe(const UTubePipe&);
  UTubePipe& operator=(const UTubePipe&);
  virtual ~UTubePipe();

  void setFront(const attachSystem::FixedComp&,const long int,const bool =0);
  void setBack(const attachSystem::FixedComp&,const long int,const bool =0);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
