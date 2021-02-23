/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/DiffPump.h
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
#ifndef xraySystem_DiffPump_h
#define xraySystem_DiffPump_h

class Simulation;

namespace xraySystem
{

/*!
  \class DiffPump
  \version 1.0
  \author Stuart Ansell
  \date February 2021
  \brief DP-differential pump by XIA
*/

class DiffPump :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double length;                 ///< Main box length
  double width;                  ///< Main box width
  double height;                 ///< Main box height

  double innerLength;            ///< Main box width
  
  double captureWidth;           ///< Main box width
  double captureHeight;         ///< Main box height

  double magWidth;               ///< Magnet Width
  double magHeight;              ///< Magnet Height

  double innerRadius;            ///< inner pipe radius
  double innerThick;             ///< inner pipe thickness

  double portLength;             ///< Port length
  double portRadius;             ///< port Radius 
  double portThick;              ///< thickness of pipe

  double flangeRadius;           ///< Flange radius
  double flangeLength;           ///< Flange thickness


  int voidMat;               ///< Void Material
  int pipeMat;               ///< Pipe Material
  int mainMat;               ///< Support Material
  int magnetMat;             ///< Magnet material
  int flangeMat;             ///< Flange material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DiffPump(const std::string&);
  DiffPump(const DiffPump&);
  DiffPump& operator=(const DiffPump&);
  virtual ~DiffPump();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif


