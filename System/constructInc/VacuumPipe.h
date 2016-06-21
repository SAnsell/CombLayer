/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/VacuumPipe.h
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
#ifndef constructSystem_VacuumPipe_h
#define constructSystem_VacuumPipe_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class VacuumPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief VacuumPipe unit  
*/

class VacuumPipe :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  const int vacIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  bool activeFront;             ///< Flag for front active
  bool activeBack;              ///< Flag for back active
  bool activeDivide;            ///< Flag for back active

  
  HeadRule frontSurf;           ///< Front surfaces [if used]
  HeadRule backSurf;            ///< Back surfaces [if used]
  HeadRule divideSurf;          ///< divider surfaces [if used]

  bool frontJoin;               ///< Flag for front join
  Geometry::Vec3D FPt;          ///< Front point
  Geometry::Vec3D FAxis;          ///< Front point

  bool backJoin;               ///< Flag for front join
  Geometry::Vec3D BPt;          ///< Front point
  Geometry::Vec3D BAxis;          ///< Front point

  double radius;                ///< void height [top only]
  double length;                ///< void length [total]

  double feThick;               ///< pipe thickness

  double flangeRadius;          ///< Joining Flange thick
  double flangeLength;          ///< Joining Flange length

  int activeWindow;             ///< Flag on window activity
  double windowThick;           ///< Joining Flange length
  double windowRadius;          ///< Joining Flange length
  
  int voidMat;                  ///< Void material
  int feMat;                    ///< Pipe material 
  int windowMat;                ///< Window material 
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void getShiftedSurf(const HeadRule&,const int,const int,const double);
  void applyActiveFrontBack();
  
 public:

  VacuumPipe(const std::string&);
  VacuumPipe(const VacuumPipe&);
  VacuumPipe& operator=(const VacuumPipe&);
  virtual ~VacuumPipe();


  void setFront(const attachSystem::FixedComp&,const long int,const bool =0);
  void setBack(const attachSystem::FixedComp&,const long int,const bool =0);
  void setDivider(const attachSystem::FixedComp&,const long int);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
