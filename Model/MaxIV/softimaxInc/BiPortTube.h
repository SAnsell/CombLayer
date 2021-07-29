/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/BiPortTube.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell / Konstantin Batkov
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
#ifndef xraySystem_BiPortTube_h
#define xraySystem_BiPortTube_h

class Simulation;

namespace xraySystem
{

/*!
  \class BiPortTube
  \version 1.0
  \author S. Ansell / K. Batkov
  \date November 2019
  \brief BiPortTube unit (PipeTube with which can be connected to two
  branches of a beam line)
*/

class BiPortTube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap

{
 protected:

  double radius;              ///< radius of main tube
  double wallThick;           ///< wall thickness of main tube
  double length;              ///< Main length

  double flangeRadius;        ///< Vertical Flange radius
  double flangeLength;        ///< Vertical Flange length
  double capThick;            ///< Vertical Cap thickness

  double outLength;           ///< Flange on outlet port
  
  double inPortRadius;        ///< Radius on inlet port
  double outPortRadius;       ///< Radius on outlet port

  double inWallThick;         ///< Wall thickness on inlet port
  double outWallThick;        ///< Wall thickness on outlet port



  double inFlangeRadius;      ///< Flange on inlet port
  double outFlangeRadius;     ///< Flange on outlet port


  double inFlangeLength;      ///< Flange on inlet port
  double outFlangeLength;     ///< Flange on outlet port

  int voidMat;                ///< void material
  int wallMat;                ///< Fe material layer
  int capMat;                 ///< flange cap material layer

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BiPortTube(const std::string&);
  BiPortTube(const BiPortTube&);
  BiPortTube& operator=(const BiPortTube&);
  virtual ~BiPortTube();

  void setLeftPort(const attachSystem::FixedComp&,const long int);
  void setRightPort(const attachSystem::FixedComp&,const long int);
  void setLeftPort(const attachSystem::FixedComp&,const std::string&);
  void setRightPort(const attachSystem::FixedComp&,const std::string&);
  
  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
