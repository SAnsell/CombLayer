/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/SplitFlangePipe.h
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
#ifndef constructSystem_SplitFlangePipe_h
#define constructSystem_SplitFlangePipe_h

class Simulation;

namespace constructSystem
{

/*!
  \class SplitFlangePipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief SplitFlangePipe unit [simplified round pipe]
*/

class SplitFlangePipe :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  const bool innerLayer;        ///< inner Fe layer [flag] -- make template
  
  const int vacIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  bool frontJoin;               ///< Flag for front join
  Geometry::Vec3D FPt;          ///< Front point
  Geometry::Vec3D FAxis;        ///< Front point

  bool backJoin;                ///< Flag for front join
  Geometry::Vec3D BPt;          ///< Front point for join
  Geometry::Vec3D BAxis;        ///< Front axis for join

  double radius;                ///< void radius [inner]
  
  double length;                ///< void length [total]

  double feThick;               ///< pipe thickness

  double bellowThick;           ///< Thickness of bellow effect
  double bellowStep;            ///< Thickness of bellow effect

  double flangeARadius;          ///< Joining Flange radius [-ve for rect]
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius [-ve for rect]
  double flangeBLength;          ///< Joining Flange length
  
  int voidMat;                  ///< Void material
  int feMat;                    ///< Pipe material
  int bellowMat;                ///< Pipe material (fractional density)
  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void applyActiveFrontBack();
  
 public:

  SplitFlangePipe(const std::string&,const bool);
  SplitFlangePipe(const SplitFlangePipe&);
  SplitFlangePipe& operator=(const SplitFlangePipe&);
  virtual ~SplitFlangePipe();

  void setFront(const attachSystem::FixedComp&,const long int,const bool =0);
  void setBack(const attachSystem::FixedComp&,const long int,const bool =0);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
