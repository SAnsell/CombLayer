/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/RoundMonoShutter.h
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
#ifndef xraySystem_RoundMonoShutter_h
#define xraySystem_RoundMonoShutter_h

class Simulation;

namespace xraySystem
{

  class FourPortTube;
  class ShutterUnit;
/*!
  \class RoundMonoShutter
  \author S. Ansell
  \version 1.0
  \date February 2019
  \brief Monochromatic shutter system 
*/

class RoundMonoShutter :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:

  double divideStep;    ///< divider step
  double divideThick;   ///< Thickness of divider
  double divideRadius;  ///< Inner hole in divider
  
  int dMat;             ///< Divider material
  
  /// Shutter pipe
  std::shared_ptr<xraySystem::FourPortTube> shutterPipe;
  /// Port A for shutter
  std::shared_ptr<constructSystem::portItem> portA;
  /// Port B for shutter
  std::shared_ptr<constructSystem::portItem> portB;
  /// shutter to stop beam
  std::shared_ptr<xraySystem::ShutterUnit> monoShutterA;
  /// shutter to stop beam [second for redundency
  std::shared_ptr<xraySystem::ShutterUnit> monoShutterB;
  // Functions:

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
  void buildComponents(Simulation&);
  
 public:

  RoundMonoShutter(const std::string&);
  RoundMonoShutter(const RoundMonoShutter&);
  RoundMonoShutter& operator=(const RoundMonoShutter&);
  virtual ~RoundMonoShutter();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);
  
};

}

#endif
 
