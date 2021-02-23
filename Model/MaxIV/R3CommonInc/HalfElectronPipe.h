/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3ComonInc/HalfElectronPipe.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_HalfElectronPipe_h
#define xraySystem_HalfElectronPipe_h

class Simulation;

namespace xraySystem
{

/*!
  \class HalfElectronPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief HalfElectronPipe unit
*/

class HalfElectronPipe :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:


  double photonRadius;            ///< round on phont side
  double electronRadius;          ///< large curve for electron 
  double wallThick;               ///< pipe thickness

  double electronAngle;           ///< Electron bend angle
  double fullLength;              ///< Full length
  
  int voidMat;                  ///< Void material
  int wallMat;                  ///< Pipe material

  Geometry::Vec3D endPoint;     ///< end point 
  Geometry::Vec3D elecAxis;     ///< axis at end point
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

public:
  
  HalfElectronPipe(const std::string&);
  HalfElectronPipe(const HalfElectronPipe&);
  HalfElectronPipe& operator=(const HalfElectronPipe&);
  virtual ~HalfElectronPipe();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
