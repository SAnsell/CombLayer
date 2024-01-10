/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1Pipe.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_M1Pipe_h
#define xraySystem_M1Pipe_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1Pipe
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focusable mirror in mount
*/

class M1Pipe :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double pipeRadius;     ///< Pipe radius (outer)

  double connectRadius;   ///< radius (outer)
  double connectLength;   ///< distance down

  double cubeWidth;      ///<
  double cubeHeight;     ///<
  double cubeDepth;      ///<  Y direction down beam

  double outRadius;   ///< radius (outer)
  double outLength;   ///< distance down

  double flangeRadius;   ///< radius (outer)
  double flangeLength;   ///< radius (outer)
  
  double exitLen;         ///< straight section after flangle
  double exitAngle;       ///< angle to rotate (about Z)
  double exitRadius;      ///< new radius at bend
  double exitFullLength;  ///< length after bend

  int pipeMat;            ///< Electron shield material
  int innerMat;           ///< void material  
  int voidMat;            ///< void material  

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1Pipe(const std::string&);
  M1Pipe(const M1Pipe&);
  M1Pipe& operator=(const M1Pipe&);
  ~M1Pipe() override;
  void adjustExtraVoids(Simulation&,const int,const int,const int);
  
  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
