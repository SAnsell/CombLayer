/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/JawFlange.h
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
#ifndef constructSystem_JawFlange_h
#define constructSystem_JawFlange_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class JawFlange
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief JawFlange unit  
*/

class JawFlange :
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Void length
  double radius;                ///< Void radius

  double jOpen;                 ///< Jaw-gap
  double jYStep;                ///< Step in beam direction
  double jThick;                ///< Blade thickness [beam]
  double jHeight;               ///< Height in flange direction
  double jWidth;                ///< length beam x flange

  int voidMat;                  ///< Void material
  int jawMat;                   ///< Jaw material

  HeadRule cylRule;             ///< Cylinder/Surround rule
  int cutCell;                  ///< Cell to cut
  
  void calcBeamCentre();
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int,
			const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  JawFlange(const std::string&);
  JawFlange(const JawFlange&);
  JawFlange& operator=(const JawFlange&);
  virtual ~JawFlange();

  void setFillRadius(const attachSystem::FixedComp&,
		     const long int,const int);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
