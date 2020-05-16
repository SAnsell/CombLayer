/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/OffsetFlangePipe.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef constructSystem_OffsetFlangePipe_h
#define constructSystem_OffsetFlangePipe_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class OffsetFlangePipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief OffsetFlangePipe unit  
*/

class OffsetFlangePipe :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
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

  double radius;                ///< void radius [inner]
  double length;                ///< void length [total]
  
  double feThick;               ///< pipe thickness

  double flangeAXStep;           ///< Joining Flange XStep
  double flangeAZStep;           ///< Joining Flange ZStep
  double flangeAXYAngle;         ///< Joining Flange angle xy
  double flangeAZAngle;          ///< Joining Flange angle z
  double flangeARadius;          ///< Joining Flange radius [-ve for rect]
  double flangeALength;          ///< Joining Flange length

  double flangeBXStep;           ///< Joining Flange XStep
  double flangeBZStep;           ///< Joining Flange ZStep
  double flangeBXYAngle;         ///< Joining Flange angle xy
  double flangeBZAngle;          ///< Joining Flange angle z
  double flangeBRadius;          ///< Joining Flange radius [-ve for rect]
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

  OffsetFlangePipe(const std::string&);
  OffsetFlangePipe(const OffsetFlangePipe&);
  OffsetFlangePipe& operator=(const OffsetFlangePipe&);
  virtual ~OffsetFlangePipe();

  void setJoinFront(const attachSystem::FixedComp&,const long int);
  void setJoinBack(const attachSystem::FixedComp&,const long int);

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 
